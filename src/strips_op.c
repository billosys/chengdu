/***
 * cpddl
 * -------
 * Copyright (c)2016 Daniel Fiser <danfis@danfis.cz>,
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
                && o1->pre.size == o2->pre.size
                && memcmp(o1->pre.fact, o2->pre.fact,
                          sizeof(int) * o1->pre.size) == 0;
    }
    return 0;
}

_bor_inline uint64_t nameHash(const char *name)
{
    return borHashDJB2(name);
}

void pddlStripsOpFree(pddl_strips_op_t *op)
{
    if (op->name)
        BOR_FREE((char *)op->name);
    pddlFactIdArrFree(&op->pre);
    pddlFactIdArrFree(&op->del_eff);
    pddlFactIdArrFree(&op->add_eff);
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
    int i;

    borHTableDel(ops->htable);
    for (i = 0; i < ops->op_size; ++i)
        pddlStripsOpFree(ops->op + i);
}

void pddlStripsAddFromPDDL(pddl_strips_ops_t *ops,
                           const pddl_t *pddl,
                           const pddl_action_t *action,
                           const int *args)
{
}
