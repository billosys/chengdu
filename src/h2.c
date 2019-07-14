/***
 * cpddl
 * -------
 * Copyright (c)2017 Daniel Fiser <danfis@danfis.cz>,
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
#include "pddl/critical_path.h"

#define REACHED 1
#define MUTEX -1
#define _FACT(h2, x, y) ((h2)->fact[(x) * (h2)->fact_size + (y)])

struct h2 {
    char *fact;
    int fact_size;
    int op_size;
    char *op_applied;
    char *op_fact;
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

_bor_inline int isUnreached(const h2_t *h2, int f1, int f2)
{
    return _FACT(h2, f1, f2) == 0;
}

_bor_inline int isReached(const h2_t *h2, int f1, int f2)
{
    return _FACT(h2, f1, f2) == REACHED;
}

_bor_inline int isMutex(const h2_t *h2, int f1, int f2)
{
    return _FACT(h2, f1, f2) == MUTEX;
}

static void h2Init(h2_t *h2,
                   const pddl_strips_t *strips,
                   const pddl_mutex_pairs_t *mutexes,
                   int use_excess_mem,
                   bor_err_t *err)
{
    bzero(h2, sizeof(*h2));
    h2->fact_size = strips->fact.fact_size;
    h2->op_size = strips->op.op_size;
    h2->fact = BOR_CALLOC_ARR(char, (size_t)h2->fact_size * h2->fact_size);
    h2->op_applied = BOR_CALLOC_ARR(char, h2->op_size);

    if (use_excess_mem){
        size_t op_fact_size = (size_t)h2->fact_size * h2->op_size;
        h2->op_fact = calloc(op_fact_size, 1);
        if (h2->op_fact != NULL){
            BOR_INFO(err, "  h^2 uses additional memory of %.2f MB",
                     op_fact_size / (1024. * 1024.));
            for (int op_id = 0; op_id < strips->op.op_size; ++op_id){
                const pddl_strips_op_t *op = strips->op.op[op_id];
                char *fact = h2->op_fact + (size_t)op_id * h2->fact_size;
                int fact_id;
                BOR_ISET_FOR_EACH(&op->add_eff, fact_id)
                    fact[fact_id] = -1;
                BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
                    fact[fact_id] = -1;
            }
        }
    }

    // Set up initial state
    int f1, f2;
    BOR_ISET_FOR_EACH(&strips->init, f1){
        BOR_ISET_FOR_EACH(&strips->init, f2){
            setReached(h2, f1, f2);
        }
    }

    // Copy mutexes into the table
    PDDL_MUTEX_PAIRS_FOR_EACH(mutexes, f1, f2)
        setMutex(h2, f1, f2);
}

static void h2Free(h2_t *h2)
{
    if (h2->fact != NULL)
        BOR_FREE(h2->fact);
    if (h2->op_applied != NULL)
        BOR_FREE(h2->op_applied);
    if (h2->op_fact != NULL)
        free(h2->op_fact);
}

/** Returns true if operator is applicable with the currently reachable facts */
static int isApplicable(const pddl_strips_op_t *op, h2_t *h2)
{
    int f1, f2;

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
            op_fact = h2->op_fact + (size_t)op->id * h2->fact_size;
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

static int h2Run(h2_t *h2, const pddl_strips_ops_t *ops, bor_err_t *err)
{
    int updated;

    do {
        updated = 0;
        for (int op_id = 0; op_id < ops->op_size; ++op_id){
            const pddl_strips_op_t *op = ops->op[op_id];
            updated |= applyOp(op, h2);
        }
    } while (updated);

    for (int f1 = 0; f1 < h2->fact_size; ++f1){
        for (int f2 = f1; f2 < h2->fact_size; ++f2){
            if (isUnreached(h2, f1, f2))
                setMutex(h2, f1, f2);
        }
    }

    return 0;
}

static void outUnreachableOps(const h2_t *h2, bor_iset_t *unreachable_ops)
{
    for (int op_id = 0; op_id < h2->op_size; ++op_id){
        if (!h2->op_applied[op_id])
            borISetAdd(unreachable_ops, op_id);
    }
}

static void outMutexes(const h2_t *h2,
                       pddl_mutex_pairs_t *mutexes,
                       bor_iset_t *unreachable_facts)
{
    for (int f1 = 0; f1 < h2->fact_size; ++f1){
        for (int f2 = f1; f2 < h2->fact_size; ++f2){
            if (isMutex(h2, f1, f2)){
                pddlMutexPairsAdd(mutexes, f1, f2);
                if (f1 == f2 && unreachable_facts != NULL)
                    borISetAdd(unreachable_facts, f1);
            }
        }
    }
}

static void setOutput(const h2_t *h2,
                      pddl_mutex_pairs_t *mutexes,
                      bor_iset_t *unreachable_facts,
                      bor_iset_t *unreachable_ops)
{
    outMutexes(h2, mutexes, unreachable_facts);
    if (unreachable_ops != NULL)
        outUnreachableOps(h2, unreachable_ops);
}


int pddlH2(const pddl_strips_t *strips,
           pddl_mutex_pairs_t *m,
           bor_iset_t *unreachable_facts,
           bor_iset_t *unreachable_ops,
           bor_err_t *err)
{
    if (strips->has_cond_eff)
        BOR_ERR_RET2(err, -1, "h^2: Conditional effects not supported!");

    BOR_INFO(err, "h^2. facts: %d, ops: %d, mutex pairs: %lu",
             strips->fact.fact_size,
             strips->op.op_size,
             (unsigned long)m->num_mutex_pairs);

    h2_t h2;

    // TODO: Check return value
    h2Init(&h2, strips, m, 1, err);

    if (h2Run(&h2, &strips->op, err) != 0)
        BOR_TRACE_RET(err, -1);

    setOutput(&h2, m, unreachable_facts, unreachable_ops);

    BOR_INFO(err, "h^2 DONE. mutex pairs: %lu, unreachable facts: %d,"
                  " unreachable ops: %d",
             (unsigned long)m->num_mutex_pairs,
             (unreachable_facts != NULL ? borISetSize(unreachable_facts) : -1),
             (unreachable_ops != NULL ? borISetSize(unreachable_ops) : -1));

    h2Free(&h2);
    return 0;
}
