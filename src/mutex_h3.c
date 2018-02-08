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

struct set_range_pair {
    fact_id_t from;
    fact_id_t to;
} bor_packed;
typedef struct set_range_pair set_range_pair_t;

struct set_range {
    set_range_pair_t *v;
    fact_id_t size;
    fact_id_t alloc;
} bor_packed;
typedef struct set_range set_range_t;

_bor_inline int setRangeIsSet(const set_range_t *s, int v)
{
    int left = 0, right = s->size - 1;
    int idx = (left + right) / 2;
    while (left <= right){
        if (s->v[idx].from <= v && v <= s->v[idx].to){
            return 1;
        }else if (v < s->v[idx].from){
            right = idx - 1;
        }else{ // f3 > mf->range[idx].to
            left = idx + 1;
        }

        idx = (left + right) / 2;
    }
    return 0;
}

_bor_inline void setRangeAlloc(set_range_t *s)
{
    if (s->size == s->alloc){
        if (s->alloc == 0)
            s->alloc = 1;
        s->alloc *= 2;
        s->v = BOR_REALLOC_ARR(s->v, set_range_pair_t, s->alloc);
    }
}

_bor_inline void setRangeSet(set_range_t *s, int v)
{
    for (int i = 0; i < s->size; ++i){
        if (s->v[i].from >= v && v >= s->v[i].to){
            return;

        }else if (s->v[i].from == v + 1){
            s->v[i].from = v;
            if (i > 0 && s->v[i - 1].to >= s->v[i].from - 1){
                s->v[i - 1].to = s->v[i].to;
                for (; i < s->size - 1; ++i)
                    s->v[i] = s->v[i + 1];
                --s->size;
            }
            return;

        }else if (s->v[i].to == v - 1){
            s->v[i].to = v;
            if (i < s->size - 1
                    && s->v[i].to >= s->v[i + 1].from - 1){
                s->v[i].to = s->v[i + 1].to;
                for (++i; i < s->size - 1; ++i)
                    s->v[i] = s->v[i + 1];
                --s->size;
            }
            return;

        }else if (s->v[i].from > v){
            setRangeAlloc(s);
            for (int j = s->size; j > i; --j)
                s->v[j] = s->v[j - 1];
            s->v[i].from = s->v[i].to = v;
            ++s->size;
            return;
        }
    }

    setRangeAlloc(s);
    s->v[s->size].from = s->v[s->size].to = v;
    ++s->size;
}

struct h3 {
    char *meta_fact1;
    char *meta_fact2;
    char *meta_fact3;
    set_range_t *meta_fact3_set;
    char *op_fact1;
    char *op_fact2;
    int *ext;
    int fact_size;
    int *op_applied;
    const int *op_unreachable;
};
typedef struct h3 h3_t;

// Assumes f1 < f2 < f3
_bor_inline int metaFactIsSet3(const h3_t *h3, int f1, int f2, int f3)
{
    if (h3->meta_fact3 != NULL){
        int idx = (f1 * h3->fact_size + f2) * h3->fact_size + f3;
        return h3->meta_fact3[idx];
    }else{
        set_range_t *s = h3->meta_fact3_set + f1 * h3->fact_size + f2;
        return setRangeIsSet(s, f3);
    }
}

// Assumes f1 <= f2
_bor_inline int metaFactIsSet2(const h3_t *h3, int f1, int f2)
{
    return h3->meta_fact2[f1 * h3->fact_size + f2];
}

_bor_inline int metaFactIsSet1(const h3_t *h3, int fid)
{
    return h3->meta_fact1[fid];
}

// Assumes f1 <= f2 <= f3
_bor_inline void metaFactSet3(h3_t *h3, int f1, int f2, int f3)
{
    if (h3->meta_fact3 != NULL){
        int idx = (f1 * h3->fact_size + f2) * h3->fact_size + f3;
        h3->meta_fact3[idx] = 1;
    }else{
        set_range_t *s = h3->meta_fact3_set + f1 * h3->fact_size + f2;
        setRangeSet(s, f3);
    }
}

// Assumes f1 <= f2
_bor_inline void metaFactSet2(h3_t *h3, int f1, int f2)
{
    h3->meta_fact2[f1 * h3->fact_size + f2] = 1;
    h3->meta_fact2[f2 * h3->fact_size + f1] = 1;
}

_bor_inline void metaFactSet1(h3_t *h3, int fid)
{
    h3->meta_fact1[fid] = 1;
}

static void h3Init(h3_t *h3, const pddl_strips_t *strips,
                   const int *unreachable_op,
                   size_t max_mem)
{
    int f1, f2, f3;
    size_t meta_fact3_size, op_fact1_size, op_fact2_size;

    bzero(h3, sizeof(*h3));
    h3->fact_size = strips->fact.fact_size;
    h3->meta_fact1 = BOR_CALLOC_ARR(char, h3->fact_size);
    h3->meta_fact2 = BOR_CALLOC_ARR(char, h3->fact_size * h3->fact_size);

    meta_fact3_size  = h3->fact_size;
    meta_fact3_size *= h3->fact_size;
    meta_fact3_size *= h3->fact_size;
    if (meta_fact3_size <= max_mem){
        h3->meta_fact3 = BOR_CALLOC_ARR(char, meta_fact3_size);
        max_mem -= meta_fact3_size;
        INFO2("Using meta_fact3");
    }else{
        h3->meta_fact3_set = BOR_CALLOC_ARR(set_range_t,
                                            h3->fact_size * h3->fact_size);
    }

    op_fact1_size  = h3->fact_size;
    op_fact1_size *= strips->op.op_size;
    if (op_fact1_size <= max_mem){
        h3->op_fact1 = BOR_CALLOC_ARR(char, op_fact1_size);
        for (int opi = 0; opi < strips->op.op_size; ++opi){
            int f;
            BOR_ISET_FOR_EACH(&strips->op.op[opi]->add_eff, f)
                h3->op_fact1[opi * h3->fact_size + f] = -1;
            BOR_ISET_FOR_EACH(&strips->op.op[opi]->del_eff, f)
                h3->op_fact1[opi * h3->fact_size + f] = -1;
        }
        max_mem -= op_fact1_size;
        INFO2("Using op_fact1");
    }

    op_fact2_size  = h3->fact_size;
    op_fact2_size *= h3->fact_size;
    op_fact2_size *= strips->op.op_size;
    if (op_fact2_size <= max_mem){
        h3->op_fact2 = BOR_CALLOC_ARR(char, op_fact2_size);
        max_mem -= op_fact2_size;
        INFO2("Using op_fact2");
    }

    h3->ext = BOR_ALLOC_ARR(int, h3->fact_size);
    h3->op_applied = BOR_CALLOC_ARR(int, strips->op.op_size);
    h3->op_unreachable = unreachable_op;

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
}

static void h3Free(h3_t *h3)
{
    if (h3->meta_fact1 != NULL)
        BOR_FREE(h3->meta_fact1);
    if (h3->meta_fact2 != NULL)
        BOR_FREE(h3->meta_fact2);
    if (h3->meta_fact3 != NULL)
        BOR_FREE(h3->meta_fact3);
    if (h3->meta_fact3_set != NULL){
        for (int i = 0; i < h3->fact_size; ++i){
            for (int j = i + 1; j < h3->fact_size; ++j){
                if (h3->meta_fact3_set[i * h3->fact_size + j].v != NULL)
                    BOR_FREE(h3->meta_fact3_set[i * h3->fact_size + j].v);
            }
        }
        BOR_FREE(h3->meta_fact3_set);
    }
    if (h3->op_fact1 != NULL)
        BOR_FREE(h3->op_fact1);
    if (h3->op_fact2 != NULL)
        BOR_FREE(h3->op_fact2);
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

    if (!h3->op_applied[op->id]){
        // This needs to be run only the first time the operator is
        // applied.
        updated = addSet(h3, &op->add_eff);
    }
    // This needs to be set here because isApplicable2 depends on it
    h3->op_applied[op->id] = 1;

    if (h3->op_fact1 != NULL){
        char *fact1 = h3->op_fact1 + op->id * h3->fact_size;
        for (int f1 = 0; f1 < h3->fact_size; ++f1){
            if (fact1[f1] || !metaFactIsSet1(h3, f1))
                continue;
            fact1[f1] = testSet2(h3, &op->pre, f1);
            if (fact1[f1])
                updated |= addSet2(h3, &op->add_eff, f1);
        }

        if (h3->op_fact2 != NULL){
            char *fact2 = h3->op_fact2;
            fact2 += (size_t)op->id * (size_t)h3->fact_size * h3->fact_size;
            for (int f1 = 0; f1 < h3->fact_size; ++f1){
                if (fact1[f1] != 1)
                    continue;
                for (int f2 = f1 + 1; f2 < h3->fact_size; ++f2){
                    if (fact1[f2] != 1
                            || fact2[f1 * h3->fact_size + f2]
                            || !metaFactIsSet2(h3, f1, f2))
                        continue;
                    if (testSet3(h3, &op->pre, f1, f2)){
                        fact2[f1 * h3->fact_size + f2] = 1;
                        updated |= addSet3(h3, &op->add_eff, f1, f2);
                    }
                }
            }

        }else{
            for (int f1 = 0; f1 < h3->fact_size; ++f1){
                if (fact1[f1] != 1)
                    continue;
                for (int f2 = f1 + 1; f2 < h3->fact_size; ++f2){
                    if (fact1[f2] != 1
                            || !metaFactIsSet2(h3, f1, f2)
                            || !testSet3(h3, &op->pre, f1, f2))
                        continue;
                    updated |= addSet3(h3, &op->add_eff, f1, f2);
                }
            }
        }

    }else{
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
                if (!h3->ext[f2]
                        || !metaFactIsSet2(h3, f1, f2)
                        || !testSet3(h3, &op->pre, f1, f2))
                    continue;
                updated |= addSet3(h3, &op->add_eff, f1, f2);
            }
        }
    }

    return updated;
}

int _pddlMutexesH3(pddl_mutexes_t *ms,
                   const pddl_strips_t *strips,
                   int *unreachable_ops,
                   size_t max_mem,
                   float max_time)
{

    h3_t h3;
    int updated;
    const pddl_strips_op_t *op;
    pddl_mutex_t *m;
    bor_iset_t mgroup;

    if (strips->has_cond_eff)
        ERR_RET2(-1, "Conditional effects are not supported by h^3.");

    h3Init(&h3, strips, unreachable_ops, max_mem);

    do {
        updated = 0;
        PDDL_STRIPS_OPS_FOR_EACH(&strips->op, op)
            updated |= applyOp(op, &h3);
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
