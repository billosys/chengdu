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
#include <boruvka/sort.h>
#include "pddl/pddl.h"
#include "pddl/strips_op.h"
#include "assert.h"

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
            && borISetEq(&o1->pre, &o2->pre)
            && borISetEq(&o1->add_eff, &o2->add_eff)
            && borISetEq(&o1->del_eff, &o2->del_eff)
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

static void condEffFree(pddl_strips_op_cond_eff_t *ce)
{
    borISetFree(&ce->pre);
    borISetFree(&ce->add_eff);
    borISetFree(&ce->del_eff);
}

void pddlStripsOpFree(pddl_strips_op_t *op)
{
    if (op->name)
        BOR_FREE(op->name);
    borISetFree(&op->pre);
    borISetFree(&op->del_eff);
    borISetFree(&op->add_eff);
    for (int i = 0; i < op->cond_eff_size; ++i)
        condEffFree(&op->cond_eff[i]);
    if (op->cond_eff != NULL)
        BOR_FREE(op->cond_eff);
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

static pddl_strips_op_cond_eff_t *addCondEff(pddl_strips_op_t *op)
{
    pddl_strips_op_cond_eff_t *ce;

    if (op->cond_eff_size >= op->cond_eff_alloc){
        if (op->cond_eff_alloc == 0)
            op->cond_eff_alloc = 1;
        op->cond_eff_alloc *= 2;
        op->cond_eff = BOR_REALLOC_ARR(op->cond_eff,
                                       pddl_strips_op_cond_eff_t,
                                       op->cond_eff_alloc);
    }

    ce = op->cond_eff + op->cond_eff_size++;
    bzero(ce, sizeof(*ce));
    return ce;
}

pddl_strips_op_cond_eff_t *pddlStripsOpAddCondEff(pddl_strips_op_t *op,
                                                  const pddl_strips_op_t *f)
{
    pddl_strips_op_cond_eff_t *ce = addCondEff(op);
    borISetUnion(&ce->pre, &f->pre);
    borISetUnion(&ce->add_eff, &f->add_eff);
    borISetUnion(&ce->del_eff, &f->del_eff);
    return ce;
}

void pddlStripsOpNormalize(pddl_strips_op_t *op)
{
    borISetMinus(&op->del_eff, &op->add_eff);
    borISetMinus(&op->add_eff, &op->pre);
}

int pddlStripsOpFinalize(pddl_strips_op_t *op, char *name)
{
    op->name = name;
    op->hash = nameHash(name);
    pddlStripsOpNormalize(op);
    if (op->add_eff.size == 0 && op->del_eff.size == 0)
        return -1;
    return 0;
}

void pddlStripsOpAddEffFromOp(pddl_strips_op_t *dst,
                              const pddl_strips_op_t *src)
{
    borISetUnion(&dst->add_eff, &src->add_eff);
    borISetUnion(&dst->del_eff, &src->del_eff);
    pddlStripsOpNormalize(dst);
}

void pddlStripsOpCopy(pddl_strips_op_t *dst, const pddl_strips_op_t *src)
{
    pddl_strips_op_cond_eff_t *ce;

    pddlStripsOpCopyWithoutCondEff(dst, src);
    for (int i = 0; i < src->cond_eff_size; ++i){
        const pddl_strips_op_cond_eff_t *f = src->cond_eff + i;
        ce = addCondEff(dst);
        borISetUnion(&ce->pre, &f->pre);
        borISetUnion(&ce->add_eff, &f->add_eff);
        borISetUnion(&ce->del_eff, &f->del_eff);
    }
}

void pddlStripsOpCopyWithoutCondEff(pddl_strips_op_t *dst,
                                    const pddl_strips_op_t *src)
{
    dst->name = BOR_STRDUP(src->name);
    dst->hash = src->hash;
    dst->cost = src->cost;
    borISetUnion(&dst->pre, &src->pre);
    borISetUnion(&dst->add_eff, &src->add_eff);
    borISetUnion(&dst->del_eff, &src->del_eff);
}

void pddlStripsOpCopyDual(pddl_strips_op_t *dst, const pddl_strips_op_t *src)
{
    pddl_strips_op_cond_eff_t *ce;

    dst->name = BOR_STRDUP(src->name);
    dst->cost = src->cost;
    borISetUnion(&dst->pre, &src->del_eff);
    borISetUnion(&dst->add_eff, &src->add_eff);
    borISetUnion(&dst->del_eff, &src->pre);
    for (int i = 0; i < src->cond_eff_size; ++i){
        const pddl_strips_op_cond_eff_t *f = src->cond_eff + i;
        ce = addCondEff(dst);
        borISetUnion(&ce->pre, &f->del_eff);
        borISetUnion(&ce->add_eff, &f->add_eff);
        borISetUnion(&ce->del_eff, &f->pre);
    }
    dst->hash = src->hash;
}

void pddlStripsOpRemapFacts(pddl_strips_op_t *op, const int *remap)
{
    pddl_strips_op_cond_eff_t *ce;

    borISetRemap(&op->pre, remap);
    borISetRemap(&op->add_eff, remap);
    borISetRemap(&op->del_eff, remap);
    for (int i = 0; i < op->cond_eff_size; ++i){
        ce = op->cond_eff + i;
        borISetRemap(&ce->pre, remap);
        borISetRemap(&ce->add_eff, remap);
        borISetRemap(&ce->del_eff, remap);
    }
}

void pddlStripsOpRemoveFact(pddl_strips_op_t *op, int fact_id)
{
    int reorder = 0;

    borISetRm(&op->pre, fact_id);
    borISetRm(&op->add_eff, fact_id);
    borISetRm(&op->del_eff, fact_id);

    for (int cei = 0; cei < op->cond_eff_size; ++cei){
        pddl_strips_op_cond_eff_t *ce = op->cond_eff + cei;
        borISetRm(&ce->pre, fact_id);
        borISetRm(&ce->add_eff, fact_id);
        borISetRm(&ce->del_eff, fact_id);
        if (borISetSize(&ce->pre) == 0){
            borISetUnion(&op->add_eff, &ce->add_eff);
            borISetUnion(&op->del_eff, &ce->del_eff);
            reorder = 1;
        }
    }

    if (reorder){
        int size = 0;
        for (int cei = 0; cei < op->cond_eff_size; ++cei){
            pddl_strips_op_cond_eff_t *ce = op->cond_eff + cei;
            if (borISetSize(&ce->pre) == 0){
                condEffFree(&op->cond_eff[cei]);
            }else{
                op->cond_eff[size++] = op->cond_eff[cei];
            }
        }
        op->cond_eff_size = size;
        pddlStripsOpNormalize(op);
    }
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
    borHTableDel(ops->htable);
    for (int i = 0; i < ops->op_size; ++i){
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

void pddlStripsOpsDelOps(pddl_strips_ops_t *ops, const int *m)
{
    int ins = 0;
    for (int op_id = 0; op_id < ops->op_size; ++op_id){
        if (m[op_id]){
            borHTableErase(ops->htable, &ops->op[op_id]->htable);
            pddlStripsOpDel(ops->op[op_id]);

        }else{
            ops->op[op_id]->id = ins;
            ops->op[ins++] = ops->op[op_id];
        }
    }

    ops->op_size = ins;
}

void pddlStripsOpsRemapFacts(pddl_strips_ops_t *ops, const int *remap)
{
    for (int i = 0; i < ops->op_size; ++i)
        pddlStripsOpRemapFacts(ops->op[i], remap);
}

void pddlStripsOpPrintDebug(const struct pddl *pddl, const pddl_facts_t *fs,
                            const pddl_strips_op_t *op, FILE *fout)
{
    fprintf(fout, "  (%s), cost: %d\n", op->name, op->cost);

    fprintf(fout, "    pre:");
    pddlFactsIdSetPrintSorted(&op->pre, fs, pddl,
                              pddlFactNamePDDL, " ", "", fout);
    fprintf(fout, "\n");
    fprintf(fout, "    add:");
    pddlFactsIdSetPrintSorted(&op->add_eff, fs, pddl,
                              pddlFactNamePDDL, " ", "", fout);
    fprintf(fout, "\n");
    fprintf(fout, "    del:");
    pddlFactsIdSetPrintSorted(&op->del_eff, fs, pddl,
                              pddlFactNamePDDL, " ", "", fout);
    fprintf(fout, "\n");

    if (op->cond_eff_size > 0)
        fprintf(fout, "    cond-eff[%d]:\n", op->cond_eff_size);

    for (int j = 0; j < op->cond_eff_size; ++j){
        const pddl_strips_op_cond_eff_t *ce = op->cond_eff + j;

        fprintf(fout, "      pre:");
        pddlFactsIdSetPrintSorted(&ce->pre, fs, pddl,
                                  pddlFactNamePDDL, " ", "", fout);
        fprintf(fout, "\n");
        fprintf(fout, "      add:");
        pddlFactsIdSetPrintSorted(&ce->add_eff, fs, pddl,
                                  pddlFactNamePDDL, " ", "", fout);
        fprintf(fout, "\n");
        fprintf(fout, "      del:");
        pddlFactsIdSetPrintSorted(&ce->del_eff, fs, pddl,
                                  pddlFactNamePDDL, " ", "", fout);
        fprintf(fout, "\n");
    }
}

void pddlStripsOpsPrintDebug(const struct pddl *pddl, const pddl_facts_t *fs,
                             const pddl_strips_ops_t *ops, FILE *fout)
{
    for (int i = 0; i < ops->op_size; ++i)
        pddlStripsOpPrintDebug(pddl, fs, ops->op[i], fout);
}
