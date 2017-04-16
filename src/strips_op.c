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

static bor_htable_key_t htableKey(const bor_list_t *key, void *_)
{
    pddl_strips_op_t *op = BOR_LIST_ENTRY(key, pddl_strips_op_t, htable);
    return op->hash;
}

static int htableEq(const bor_list_t *k1,
                    const bor_list_t *k2, void *_)
{
    pddl_strips_op_t *o1 = BOR_LIST_ENTRY(k1, pddl_strips_op_t, htable);
    pddl_strips_op_t *o2 = BOR_LIST_ENTRY(k2, pddl_strips_op_t, htable);

    return o1->hash == o2->hash
            && strcmp(o1->name, o2->name) == 0
            && pddlFactIdArrEq(&o1->pre, &o2->pre)
            && pddlFactIdArrEq(&o1->add_eff, &o2->add_eff)
            && pddlFactIdArrEq(&o1->del_eff, &o2->del_eff)
            && o1->cost == o2->cost;
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
        BOR_FREE(op->name);
    pddlFactIdArrFree(&op->pre);
    pddlFactIdArrFree(&op->del_eff);
    pddlFactIdArrFree(&op->add_eff);
}

pddl_strips_op_t *pddlStripsOpNew(void)
{
    pddl_strips_op_t *op;
    op = BOR_ALLOC(pddl_strips_op_t);
    pddlStripsOpInit(op);
    return op;
}

void pddlStripsOpDel(pddl_strips_op_t *op)
{
    pddlStripsOpFree(op);
    BOR_FREE(op);
}

int pddlStripsOpFinalize(pddl_strips_op_t *op, char *name)
{
    op->name = name;
    op->hash = nameHash(name);
    pddlFactIdArrMinus(&op->del_eff, &op->add_eff);
    pddlFactIdArrMinus(&op->add_eff, &op->pre);
    if (op->add_eff.size == 0 && op->del_eff.size == 0)
        return -1;
    return 0;
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
    ops->op = BOR_ALLOC_ARR(pddl_strips_op_t *, ops->op_alloc);
    ops->htable = borHTableNew(htableKey, htableEq, NULL);
}

void pddlStripsOpsFree(pddl_strips_ops_t *ops)
{
    int i;
    borHTableDel(ops->htable);
    for (i = 0; i < ops->op_size; ++i){
        if (ops->op[i])
            pddlStripsOpDel(ops->op[i]);
    }
    if (ops->op != NULL)
        BOR_FREE(ops->op);
}

static pddl_strips_op_t *nextNewOp(pddl_strips_ops_t *ops)
{
    pddl_strips_op_t *op;

    if (ops->op_size >= ops->op_alloc){
        ops->op_alloc *= 2;
        ops->op = BOR_REALLOC_ARR(ops->op, pddl_strips_op_t *, ops->op_alloc);
    }

    op = pddlStripsOpNew();
    op->id = ops->op_size;
    ops->op[ops->op_size] = op;
    ++ops->op_size;
    return op;
}

int pddlStripsOpsAdd(pddl_strips_ops_t *ops, const pddl_strips_op_t *add)
{
    pddl_strips_op_t op_find, *op;
    bor_list_t *lfound;

    op_find = *add;
    op_find.hash = nameHash(op_find.name);
    if ((lfound = borHTableFind(ops->htable, &op_find.htable)) != NULL){
        op = bor_container_of(lfound, pddl_strips_op_t, htable);
        return op->id;
    }

    op = nextNewOp(ops);
    pddlStripsOpCopy(op, add);
    op->hash = op_find.hash;
    borHTableInsert(ops->htable, &op->htable);
    return op->id;
}
