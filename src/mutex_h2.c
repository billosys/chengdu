/***
 * cpddl
 * -------
 * Copyright (c)2017 Daniel Fiser <danfis@danfis.cz>,
 * AI Center, Department of Computer Science,
 * Faculty of Electrical Engineering, Czech Technical University in Prague.
 * All rights reserved.
 *
 * This file is part of cpddl.
 *
 * Distributed under the OSI-approved BSD License (the "License");
 * see accompanying file BDS-LICENSE for details or see
 * <http://www.opensource.org/licenses/bsd-license.php>.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the License for more information.
 */

#include <boruvka/alloc.h>
#include <boruvka/timer.h>
#include "pddl/strips.h"
#include "pddl/mutex.h"
#include "err.h"

#define REACHED 1
#define MUTEX -1
#define _FACT(h2, x, y) ((h2)->fact[(x) * (h2)->fact_size + (y)])

struct h2 {
    char *fact;
    int fact_size;
    char *op_applied;
    char *op_fact;
    const int *op_unreachable;
};
typedef struct h2 h2_t;

_bor_inline int setReached(h2_t *h2, int f1, int f2)
{
    if (_FACT(h2, f1, f2) == 0){
        _FACT(h2, f1, f2) = _FACT(h2, f2, f1) = REACHED;
        return 1;
    }
    return 0;
}

_bor_inline void setMutex(h2_t *h2, int f1, int f2)
{
    _FACT(h2, f1, f2) = _FACT(h2, f2, f1) = MUTEX;
}

_bor_inline int isReached(const h2_t *h2, int f1, int f2)
{
    return _FACT(h2, f1, f2) == REACHED;
}

_bor_inline int isMutex(const h2_t *h2, int f1, int f2)
{
    return _FACT(h2, f1, f2) == MUTEX;
}

static int checkTimer(bor_timer_t *timer, float max_time)
{
    if (max_time <= 0.f)
        return 1;

    borTimerStop(timer);
    if (borTimerElapsedInSF(timer) > max_time)
        return 0;
    return 1;
}

static void h2Init(h2_t *h2,
                   const pddl_strips_t *strips,
                   const pddl_mutexes_t *mutexes,
                   const int *unreachable_op,
                   size_t max_mem)
{
    int f1, f2;
    size_t op_fact_size;
    const pddl_mutex_t *mutex;

    bzero(h2, sizeof(*h2));
    h2->fact_size = strips->fact.fact_size;
    h2->fact = BOR_CALLOC_ARR(char, h2->fact_size * h2->fact_size);
    h2->op_applied = BOR_CALLOC_ARR(char, strips->op.op_size);
    h2->op_unreachable = unreachable_op;

    max_mem -= sizeof(*h2);
    max_mem -= h2->fact_size * h2->fact_size;
    max_mem -= strips->op.op_size;

    op_fact_size  = h2->fact_size;
    op_fact_size *= strips->op.op_size;
    if (op_fact_size <= max_mem){
        h2->op_fact = BOR_CALLOC_ARR(char, op_fact_size);
        for (int opi = 0; opi < strips->op.op_size; ++opi){
            const pddl_strips_op_t *op = strips->op.op[opi];
            char *fact = h2->op_fact + opi * h2->fact_size;
            BOR_ISET_FOR_EACH(&op->add_eff, f1)
                fact[f1] = -1;
            BOR_ISET_FOR_EACH(&op->del_eff, f1)
                fact[f1] = -1;
        }
    }

    // Copy mutexes into the table
    PDDL_MUTEXES_FOR_EACH(mutexes, mutex){
        if (borISetSize(&mutex->fact) == 1){
            f1 = borISetGet(&mutex->fact, 0);
            setMutex(h2, f1, f1);
        }else if (borISetSize(&mutex->fact) == 2){
            f1 = borISetGet(&mutex->fact, 0);
            f2 = borISetGet(&mutex->fact, 1);
            setMutex(h2, f1, f2);
        }
    }

    // Set up initial state
    BOR_ISET_FOR_EACH(&strips->init, f1){
        BOR_ISET_FOR_EACH(&strips->init, f2){
            setReached(h2, f1, f2);
        }
    }
}

static void h2Free(h2_t *h2)
{
    if (h2->fact != NULL)
        BOR_FREE(h2->fact);
    if (h2->op_applied != NULL)
        BOR_FREE(h2->op_applied);
    if (h2->op_fact != NULL)
        BOR_FREE(h2->op_fact);
}

/** Returns true if operator is applicable with the currently reachable facts */
static int isApplicable(const pddl_strips_op_t *op, h2_t *h2)
{
    int f1, f2;

    if (h2->op_unreachable != NULL && h2->op_unreachable[op->id])
        return 0;
    if (h2->op_applied[op->id])
        return 1;

    BOR_ISET_FOR_EACH(&op->pre, f1){
        BOR_ISET_FOR_EACH(&op->pre, f2){
            if (!isReached(h2, f1, f2))
                return 0;
        }
    }

    return 1;
}

/** Returns true if operator is applicable with the additional fact_id */
static int isApplicable2(const pddl_strips_op_t *op, int fact_id, h2_t *h2)
{
    int f1;

    if (h2->op_unreachable != NULL && h2->op_unreachable[op->id])
        return 0;
    if (!h2->op_applied[op->id])
        return 0;
    if (!isReached(h2, fact_id, fact_id))
        return 0;
    if (borISetHas(&op->add_eff, fact_id) || borISetHas(&op->del_eff, fact_id))
        return 0;

    BOR_ISET_FOR_EACH(&op->pre, f1){
        if (!isReached(h2, f1, fact_id))
            return 0;
    }

    return 1;
}

/** Apply operator if currently applicable */
static int applyOp(const pddl_strips_op_t *op, h2_t *h2)
{
    int f1, f2;
    int updated = 0;
    char *op_fact = NULL;

    if (!isApplicable(op, h2))
        return 0;

    if (!h2->op_applied[op->id]){
        // This needs to be run only the first time the operator is
        // applied.
        BOR_ISET_FOR_EACH(&op->add_eff, f1){
            BOR_ISET_FOR_EACH(&op->add_eff, f2){
                updated |= setReached(h2, f1, f2);
            }
        }
        // This needs to be set here because isApplicable2 depends on it
        h2->op_applied[op->id] = 1;
    }

    for (int fact_id = 0; fact_id < h2->fact_size; ++fact_id){
        if (h2->op_fact != NULL)
            op_fact = h2->op_fact + op->id * h2->fact_size;
        if (op_fact != NULL && op_fact[fact_id])
            continue;
        if (isApplicable2(op, fact_id, h2)){
            if (op_fact != NULL)
                op_fact[fact_id] = 1;
            BOR_ISET_FOR_EACH(&op->add_eff, f1)
                updated |= setReached(h2, f1, fact_id);
        }
    }

    return updated;
}

int _pddlMutexesH2(pddl_mutexes_t *ms,
                   const pddl_strips_t *strips,
                   int *unreachable_ops,
                   size_t max_mem,
                   float max_time)
{
    bor_timer_t timer;
    h2_t h2;
    int updated, ret = 0;
    const pddl_strips_op_t *op;
    pddl_mutex_t *m;
    bor_iset_t mgroup;

    if (strips->has_cond_eff)
        ERR_RET2(-1, "Conditional effects are not supported by h^2.");

    borTimerStart(&timer);
    h2Init(&h2, strips, ms, unreachable_ops, max_mem);

    do {
        if (!checkTimer(&timer, max_time)){
            ret = -2;
            goto mutex_h2_end;
        }

        updated = 0;
        PDDL_STRIPS_OPS_FOR_EACH(&strips->op, op)
            updated |= applyOp(op, &h2);
    } while (updated);

    if (ms != NULL){
        borISetInit(&mgroup);
        for (int f1 = 0; f1 < h2.fact_size; ++f1){
            for (int f2 = f1; f2 < h2.fact_size; ++f2){
                if (!isReached(&h2, f1, f2) && !isMutex(&h2, f1, f2)){
                    borISetEmpty(&mgroup);
                    borISetAdd(&mgroup, f1);
                    borISetAdd(&mgroup, f2);
                    m = pddlMutexesAdd(ms, &mgroup);
                    m->hm = borISetSize(&mgroup);
                }
            }
        }
        borISetFree(&mgroup);
    }

    if (unreachable_ops != NULL){
        for (int i = 0; i < strips->op.op_size; ++i){
            if (!h2.op_applied[i])
                unreachable_ops[i] = 1;
        }
    }

mutex_h2_end:
    h2Free(&h2);

    return ret;
}
