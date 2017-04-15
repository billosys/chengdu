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

#include <boruvka/hfunc.h>
#include "pddl/strips_op.h"

struct htable_op {
    int id;
    uint64_t hash;
    bor_list_t htable;
};
typedef struct htable_op htable_op_t;


static bor_htable_key_t htableKey(const bor_list_t *key, void *_)
{
    htable_op_t *h;
    h = BOR_LIST_ENTRY(key, htable_op_t, htable);
    return h->hash;
}

static int htableEq(const bor_list_t *k1,
                    const bor_list_t *k2, void *_ops)
{
    pddl_strips_ops_t *ops = _ops;
    htable_op_t *ho1 = BOR_LIST_ENTRY(k1, htable_op_t, htable);
    htable_op_t *ho2 = BOR_LIST_ENTRY(k2, htable_op_t, htable);
    pddl_strips_op_t *o1, *o2;

    if (ho1->hash != ho2->hash){
        o1 = ops->op + ho1->id;
        o2 = ops->op + ho2->id;
        return strcmp(o1->name, o2->name) == 0
                && pddlFactIdArrEq(&o1->pre, &o2->pre)
                && pddlFactIdArrEq(&o1->add_eff, &o2->add_eff)
                && pddlFactIdArrEq(&o1->del_eff, &o2->del_eff)
                && o1->cost == o2->cost;
    }
    return 0;
}

_bor_inline uint64_t nameHash(const char *name)
{
    return borHashDJB2(name);
}

void pddlStripsOpInit(pddl_strips_op_t *op)
{
    bzero(op, sizeof(*op));
}

void pddlStripsOpFree(pddl_strips_op_t *op)
{
    if (op->name)
        BOR_FREE((char *)op->name);
    pddlFactIdArrFree(&op->pre);
    pddlFactIdArrFree(&op->del_eff);
    pddlFactIdArrFree(&op->add_eff);
}

void pddlStripsOpCopy(pddl_strips_op_t *dst, const pddl_strips_op_t *src)
{
    dst->name = BOR_STRDUP(src->name);
    pddlFactIdArrCopy(&dst->pre, &src->pre);
    pddlFactIdArrCopy(&dst->add_eff, &src->add_eff);
    pddlFactIdArrCopy(&dst->del_eff, &src->del_eff);
}

void pddlStripsOpsInit(pddl_strips_ops_t *ops)
{
    bzero(ops, sizeof(*ops));
    ops->op_alloc = 4;
    ops->op = BOR_ALLOC_ARR(pddl_strips_op_t, ops->op_alloc);
    ops->htable = borHTableNew(htableKey, htableEq, ops);
}

void pddlStripsOpsFree(pddl_strips_ops_t *ops)
{
    bor_list_t list, *h, *tmp;
    int i;

    borListInit(&list);
    borHTableGather(ops->htable, &list);
    BOR_LIST_FOR_EACH_SAFE(&list, h, tmp){
        BOR_FREE(BOR_LIST_ENTRY(h, htable_op_t, htable));
    }
    borHTableDel(ops->htable);

    for (i = 0; i < ops->op_size; ++i)
        pddlStripsOpFree(ops->op + i);
    if (ops->op != NULL)
        BOR_FREE(ops->op);
}

static pddl_strips_op_t *nextOp(pddl_strips_ops_t *ops)
{
    if (ops->op_size >= ops->op_alloc){
        ops->op_alloc *= 2;
        ops->op = BOR_REALLOC_ARR(ops->op, pddl_strips_op_t, ops->op_alloc);
    }
    return ops->op + ops->op_size;
}

int pddlStripsOpsAdd(pddl_strips_ops_t *ops, const pddl_strips_op_t *add)
{
    pddl_strips_op_t *op;
    htable_op_t hop, *found, *hadd;
    bor_list_t *lfound;

    // Get next operators
    op = nextOp(ops);

    // Create shallow copy only for finding duplicates
    *op = *add;
    hop.id = ops->op_size;
    hop.hash = nameHash(add->name);
    if ((lfound = borHTableFind(ops->htable, &hop.htable)) != NULL){
        found = bor_container_of(lfound, htable_op_t, htable);
        return found->id;
    }

    // Make deep copy and add it to hash table
    pddlStripsOpCopy(op, add);
    hadd = BOR_ALLOC(htable_op_t);
    hadd->id = ops->op_size;
    hadd->hash = hop.hash;
    borHTableInsert(ops->htable, &hadd->htable);

    return ops->op_size++;
}
