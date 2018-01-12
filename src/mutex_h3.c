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

#include <boruvka/compiler.h>
#include <boruvka/alloc.h>
#include "pddl/strips.h"
#include "pddl/mutex.h"
#include "err.h"
#include "assert.h"

typedef int16_t fact_id_t;

struct meta_fact_range {
    fact_id_t from;
    fact_id_t to;
} bor_packed;
typedef struct meta_fact_range meta_fact_range_t;

struct meta_fact {
    meta_fact_range_t *range;
    fact_id_t size;
    fact_id_t alloc;
    char is_set;
} bor_packed;
typedef struct meta_fact meta_fact_t;

struct h3 {
    meta_fact_t *meta_fact;
    int *ext;
    int fact_size;
    int *op_applied;
    const int *op_unreachable;
};
typedef struct h3 h3_t;

// Assumes f1 < f2 < f3
_bor_inline int metaFactIsSet3(const h3_t *h3, int f1, int f2, int f3)
{
    const meta_fact_t *mf = h3->meta_fact + f1 * h3->fact_size + f2;
    for (int i = 0; i < mf->size; ++i){
        if (mf->range[i].from <= f3 && f3 <= mf->range[i].to)
            return 1;
        if (mf->range[i].from > f3)
            return 0;
    }
    return 0;
}

// Assumes f1 <= f2
_bor_inline int metaFactIsSet2(const h3_t *h3, int f1, int f2)
{
    return h3->meta_fact[f1 * h3->fact_size + f2].is_set; 
}

_bor_inline int metaFactIsSet1(const h3_t *h3, int fid)
{
    return metaFactIsSet2(h3, fid, fid);
}

// Assumes f1 <= f2 <= f3
_bor_inline void metaFactSet3(h3_t *h3, int f1, int f2, int f3)
{
    meta_fact_t *mf = h3->meta_fact + f1 * h3->fact_size + f2;
    for (int i = 0; i < mf->size; ++i){
        if (mf->range[i].from >= f3 && f3 >= mf->range[i].to){
            return;

        }else if (mf->range[i].from == f3 + 1){
            mf->range[i].from = f3;
            if (i > 0
                    && mf->range[i - 1].to >= mf->range[i].from - 1){
                mf->range[i - 1].to = mf->range[i].to;
                for (; i < mf->size - 1; ++i)
                    mf->range[i] = mf->range[i + 1];
                --mf->size;
            }
            return;

        }else if (mf->range[i].to == f3 - 1){
            mf->range[i].to = f3;
            if (i < mf->size - 1
                    && mf->range[i].to >= mf->range[i + 1].from - 1){
                mf->range[i].to = mf->range[i + 1].to;
                for (++i; i < mf->size - 1; ++i)
                    mf->range[i] = mf->range[i + 1];
                --mf->size;
            }
            return;

        }else if (mf->range[i].from > f3){
            if (mf->size == mf->alloc){
                if (mf->alloc == 0)
                    mf->alloc = 1;
                mf->alloc *= 2;
                mf->range = BOR_REALLOC_ARR(mf->range, meta_fact_range_t,
                                            mf->alloc);
            }
            for (int j = mf->size; j > i; --j)
                mf->range[j] = mf->range[j - 1];
            mf->range[i].from = mf->range[i].to = f3;
            ++mf->size;
            return;
        }
    }

    if (mf->size == mf->alloc){
        if (mf->alloc == 0)
            mf->alloc = 1;
        mf->alloc *= 2;
        mf->range = BOR_REALLOC_ARR(mf->range, meta_fact_range_t, mf->alloc);
    }
    mf->range[mf->size].from = mf->range[mf->size].to = f3;
    ++mf->size;
}

// Assumes f1 <= f2
_bor_inline void metaFactSet2(h3_t *h3, int f1, int f2)
{
    h3->meta_fact[f1 * h3->fact_size + f2].is_set = 1;
}

_bor_inline void metaFactSet1(h3_t *h3, int fid)
{
    metaFactSet2(h3, fid, fid);
}

static void h3Init(h3_t *h3, const pddl_strips_t *strips,
                   const int *unreachable_op)
{
    int f1, f2, f3;

    bzero(h3, sizeof(*h3));
    h3->fact_size = strips->fact.fact_size;
    h3->meta_fact = BOR_CALLOC_ARR(meta_fact_t, h3->fact_size * h3->fact_size);
    h3->ext = BOR_ALLOC_ARR(int, h3->fact_size);
    h3->op_applied = BOR_CALLOC_ARR(int, strips->op.op_size);
    h3->op_unreachable = unreachable_op;

    /*
    BOR_ISET_FOR_EACH(&strips->init, f1)
        fprintf(stderr, " %d", f1);
    fprintf(stderr, "\n");
    */

    for (int i = 0; i < borISetSize(&strips->init); ++i){
        f1 = borISetGet(&strips->init, i);
        metaFactSet1(h3, f1);
        for (int j = i + 1; j < borISetSize(&strips->init); ++j){
            f2 = borISetGet(&strips->init, j);
            metaFactSet2(h3, f1, f2);
            for (int k = j + 1; k < borISetSize(&strips->init); ++k){
                f3 = borISetGet(&strips->init, k);
                metaFactSet3(h3, f1, f2, f3);
            }
        }
    }

    /*
    for (int i = 0; i < borISetSize(&strips->init); ++i){
        f1 = borISetGet(&strips->init, i);
        ASSERT(metaFactIsSet1(h3, f1));
        for (int j = i + 1; j < borISetSize(&strips->init); ++j){
            f2 = borISetGet(&strips->init, j);
            ASSERT(metaFactIsSet2(h3, f1, f2));
            for (int k = j + 1; k < borISetSize(&strips->init); ++k){
                f3 = borISetGet(&strips->init, k);
                ASSERT(metaFactIsSet3(h3, f1, f2, f3));
            }
        }
    }

    for (int i = 0; i < borISetSize(&strips->init); ++i){
        f1 = borISetGet(&strips->init, i);
        ASSERT(metaFactIsSet1(h3, f1));
        for (int j = 0; j < borISetSize(&strips->init); ++j){
            f2 = borISetGet(&strips->init, j);
            const meta_fact_t *mf = h3->meta_fact + f1 * h3->fact_size + f2;
            fprintf(stderr, "%d/%d:%d:", f1, f2, mf->is_set);
            for (int x = 0; x < mf->size; ++x)
                fprintf(stderr, " %d:%d", mf->range[x].from,
                        mf->range[x].to);
            fprintf(stderr, "\n");
        }
    }
    */
}

static void h3Free(h3_t *h3)
{
    for (int i = 0; i < h3->fact_size; ++i){
        for (int j = i + 1; j < h3->fact_size; ++j){
            if (h3->meta_fact[i * h3->fact_size + j].range != NULL)
                BOR_FREE(h3->meta_fact[i * h3->fact_size + j].range);
        }
    }
    if (h3->meta_fact != NULL)
        BOR_FREE(h3->meta_fact);
    if (h3->ext != NULL)
        BOR_FREE(h3->ext);

    if (h3->op_applied != NULL)
        BOR_FREE(h3->op_applied);
}

static int testSet(const h3_t *h3, const bor_iset_t *set)
{
    for (int i = 0; i < borISetSize(set); ++i){
        int f1 = borISetGet(set, i);
        if (!metaFactIsSet1(h3, f1))
            return 0;
        for (int j = i + 1; j < borISetSize(set); ++j){
            int f2 = borISetGet(set, j);
            if (!metaFactIsSet2(h3, f1, f2))
                return 0;
            for (int k = j + 1; k < borISetSize(set); ++k){
                int f3 = borISetGet(set, k);
                if (!metaFactIsSet3(h3, f1, f2, f3))
                    return 0;
            }
        }
    }

    return 1;
}

static int testSet2(const h3_t *h3, const bor_iset_t *set, int f)
{
    if (borISetIn(f, set))
        return 1;

    for (int i = 0; i < borISetSize(set); ++i){
        int f1 = f;
        int f2 = borISetGet(set, i);
        if (f > f2){
            f1 = f2;
            f2 = f;
        }

        if (!metaFactIsSet2(h3, f1, f2))
            return 0;

        for (int j = i + 1; j < borISetSize(set); ++j){
            int t = borISetGet(set, j);
            int t1 = f1, t2 = f2, t3 = t;
            if (t < t2){
                t3 = t2;
                t2 = t;
            }
            if (t < t1){
                t2 = t1;
                t1 = t;
            }

            if (!metaFactIsSet3(h3, t1, t2, t3))
                return 0;
        }
    }

    return 1;
}

static int testSet3(const h3_t *h3, const bor_iset_t *set, int f1, int f2)
{
    int f;

    if (borISetIn(f1, set) || borISetIn(f2, set))
        return 1;

    BOR_ISET_FOR_EACH(set, f){
        if (f < f2){
            if (f < f1){
                if (!metaFactIsSet3(h3, f, f1, f2))
                    return 0;
            }else if (!metaFactIsSet3(h3, f1, f, f2)){
                return 0;
            }
        }else if (!metaFactIsSet3(h3, f1, f2, f)){
            return 0;
        }
    }

    return 1;
}

static int addSet(h3_t *h3, const bor_iset_t *set)
{
    int updated = 0;

    for (int i = 0; i < borISetSize(set); ++i){
        int f1 = borISetGet(set, i);
        if (!metaFactIsSet1(h3, f1)){
            metaFactSet1(h3, f1);
            updated = 1;
        }
        for (int j = i + 1; j < borISetSize(set); ++j){
            int f2 = borISetGet(set, j);
            if (!metaFactIsSet2(h3, f1, f2)){
                metaFactSet2(h3, f1, f2);
                updated = 1;
            }
            for (int k = j + 1; k < borISetSize(set); ++k){
                int f3 = borISetGet(set, k);
                if (!metaFactIsSet3(h3, f1, f2, f3)){
                    metaFactSet3(h3, f1, f2, f3);
                    updated = 1;
                }
            }
        }
    }

    return updated;
}

static int addSet2(h3_t *h3, const bor_iset_t *set, int f)
{
    int updated = 0;

    for (int i = 0; i < borISetSize(set); ++i){
        int f1 = f;
        int f2 = borISetGet(set, i);
        if (f2 < f){
            f1 = f2;
            f2 = f;
        }
        if (!metaFactIsSet2(h3, f1, f2)){
            metaFactSet2(h3, f1, f2);
            updated = 1;
        }
        for (int j = i + 1; j < borISetSize(set); ++j){
            int t = borISetGet(set, j);
            int t1 = f1, t2 = f2, t3 = t;
            if (t < t2){
                t3 = t2;
                t2 = t;
            }
            if (t < t1){
                t2 = t1;
                t1 = t;
            }
            if (!metaFactIsSet3(h3, t1, t2, t3)){
                metaFactSet3(h3, t1, t2, t3);
                updated = 1;
            }
        }
    }

    return updated;
}

static int addSet3(h3_t *h3, const bor_iset_t *set, int f1, int f2)
{
    int f;
    int updated = 0;

    BOR_ISET_FOR_EACH(set, f){
        if (f < f2){
            if (f < f1){
                if (!metaFactIsSet3(h3, f, f1, f2)){
                    metaFactSet3(h3, f, f1, f2);
                    updated = 1;
                }
            }else{
                if (!metaFactIsSet3(h3, f1, f, f2)){
                    metaFactSet3(h3, f1, f, f2);
                    updated = 1;
                }
            }
        }else{
            if (!metaFactIsSet3(h3, f1, f2, f)){
                metaFactSet3(h3, f1, f2, f);
                updated = 1;
            }
        }
    }

    return updated;
}

/** Returns true if operator is applicable with the currently reachable facts */
static int isApplicable(const pddl_strips_op_t *op, h3_t *h3)
{
    if (h3->op_unreachable != NULL && h3->op_unreachable[op->id])
        return 0;
    if (h3->op_applied[op->id])
        return 1;

    return testSet(h3, &op->pre);
}

/** Apply operator if currently applicable */
static int applyOp(const pddl_strips_op_t *op, h3_t *h3)
{
    int updated = 0;

    if (!isApplicable(op, h3))
        return 0;

    /*
    fprintf(stderr, "A %d ", op->id);
    fprintf(stderr, " pre:");
    int f;
    BOR_ISET_FOR_EACH(&op->pre, f)
        fprintf(stderr, " %d", f);
    fprintf(stderr, " add:");
    BOR_ISET_FOR_EACH(&op->add_eff, f)
        fprintf(stderr, " %d", f);
    fprintf(stderr, " del:");
    BOR_ISET_FOR_EACH(&op->del_eff, f)
        fprintf(stderr, " %d", f);
    fprintf(stderr, "\n");
    */
    if (!h3->op_applied[op->id]){
        // This needs to be run only the first time the operator is
        // applied.
        updated = addSet(h3, &op->add_eff);
    }
    // This needs to be set here because isApplicable2 depends on it
    h3->op_applied[op->id] = 1;

    bzero(h3->ext, sizeof(int) * h3->fact_size);
    for (int f1 = 0; f1 < h3->fact_size; ++f1){
        if (borISetIn(f1, &op->add_eff)
                || borISetIn(f1, &op->del_eff)
                || !metaFactIsSet1(h3, f1)
                || !testSet2(h3, &op->pre, f1))
            continue;
        updated |= addSet2(h3, &op->add_eff, f1);
        h3->ext[f1] = 1;
    }

    for (int f1 = 0; f1 < h3->fact_size; ++f1){
        if (!h3->ext[f1])
            continue;
        for (int f2 = f1 + 1; f2 < h3->fact_size; ++f2){
            if (!h3->ext[f2])
                continue;
            if (!metaFactIsSet2(h3, f1, f2) || !testSet3(h3, &op->pre, f1, f2))
                continue;
            updated |= addSet3(h3, &op->add_eff, f1, f2);
        }
    }

    return updated;
}

int _pddlMutexesH3(const pddl_strips_t *strips, pddl_mutexes_t *ms,
                   int *unreachable_ops)
{

    h3_t h3;
    int updated;
    const pddl_strips_op_t *op;
    pddl_mutex_t *m;
    bor_iset_t mgroup;

    if (strips->has_cond_eff)
        ERR_RET2(-1, "Conditional effects are not supported by h^3.");

    h3Init(&h3, strips, unreachable_ops);

    do {
        updated = 0;
        INFO2("CYCLE");
        PDDL_STRIPS_OPS_FOR_EACH(&strips->op, op){
            updated |= applyOp(op, &h3);
        }
    } while (updated);

    if (ms != NULL){
        borISetInit(&mgroup);
        for (int f1 = 0; f1 < h3.fact_size; ++f1){
            if (!metaFactIsSet1(&h3, f1)){
                borISetEmpty(&mgroup);
                borISetAdd(&mgroup, f1);
                m = pddlMutexesAdd(ms, &mgroup);
                m->hm = 1;
                continue;
            }

            for (int f2 = f1 + 1; f2 < h3.fact_size; ++f2){
                if (!metaFactIsSet2(&h3, f1, f2)){
                    borISetEmpty(&mgroup);
                    borISetAdd(&mgroup, f1);
                    borISetAdd(&mgroup, f2);
                    m = pddlMutexesAdd(ms, &mgroup);
                    m->hm = 2;
                    continue;
                }
                // TODO
                continue;
                for (int f3 = f2 + 1; f3 < h3.fact_size; ++f3){
                    if (!metaFactIsSet3(&h3, f1, f2, f3)){
                        borISetEmpty(&mgroup);
                        borISetAdd(&mgroup, f1);
                        borISetAdd(&mgroup, f2);
                        borISetAdd(&mgroup, f3);
                        m = pddlMutexesAdd(ms, &mgroup);
                        m->hm = 3;
                    }
                }
            }
        }
        borISetFree(&mgroup);
    }

    if (unreachable_ops != NULL){
        for (int i = 0; i < strips->op.op_size; ++i){
            if (!h3.op_applied[i])
                unreachable_ops[i] = 1;
        }
    }

    h3Free(&h3);

    return 0;
}
