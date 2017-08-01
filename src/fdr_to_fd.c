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

#include "pddl/fdr.h"

static int hasMetric(const pddl_fdr_t *fdr)
{
    for (int i = 0; i < fdr->op_size; ++i){
        if (fdr->op[i].cost != 1)
            return 1;
    }
    return 0;
}

static void printVar(const pddl_fdr_t *fdr, int id,
                     const pddl_fdr_var_t *var, FILE *fout)
{
    fprintf(fout, "begin_variable\n");
    fprintf(fout, "var%d\n", id);
    fprintf(fout, "-1\n");
    fprintf(fout, "%d\n", var->size);
    for (int i = 0; i < var->size; ++i)
        fprintf(fout, "%s\n", var->val[i].name);
    fprintf(fout, "end_variable\n");
}

static void printPartState(const pddl_fdr_part_state_t *ps, FILE *fout)
{
    int size = 0;
    for (int i = 0; i < ps->size; ++i){
        if (pddlFDRPartStateIsSet(ps, i))
            ++size;
    }

    fprintf(fout, "%d\n", size);
    for (int i = 0; i < ps->size; ++i){
        if (pddlFDRPartStateIsSet(ps, i))
            fprintf(fout, "%d %d\n", i, ps->val[i]);
    }
}

struct sas_cond {
    int *var;
    int *val;
    int size;
    int alloc;
};
typedef struct sas_cond sas_cond_t;

struct sas_eff {
    sas_cond_t cond;
    int var;
    int pre;
    int eff;
};
typedef struct sas_eff sas_eff_t;

struct sas_op {
    sas_cond_t prevail;
    sas_eff_t *eff;
    int eff_size;
    int eff_alloc;
};
typedef struct sas_op sas_op_t;

static void sasCondAdd(sas_cond_t *c, int var, int val)
{
    if (c->size == c->alloc){
        if (c->alloc == 0)
            c->alloc = 2;
        c->alloc *= 2;
        c->var = BOR_REALLOC_ARR(c->var, int, c->alloc);
        c->val = BOR_REALLOC_ARR(c->val, int, c->alloc);
    }

    c->var[c->size] = var;
    c->val[c->size] = val;
    ++c->size;
}

static sas_eff_t *sasOpAddEff(sas_op_t *op, int var, int pre, int eff)
{
    sas_eff_t *e;

    if (op->eff_size == op->eff_alloc){
        if (op->eff_alloc == 0)
            op->eff_alloc = 2;
        op->eff_alloc *= 2;
        op->eff = BOR_REALLOC_ARR(op->eff, sas_eff_t, op->eff_alloc);
    }

    e = op->eff + op->eff_size++;
    bzero(e, sizeof(*e));
    e->var = var;
    e->pre = pre;
    e->eff = eff;
    return e;
}

static void sasOpInit(sas_op_t *op, const pddl_fdr_op_t *fop)
{
    sas_eff_t *e;

    bzero(op, sizeof(*op));
    for (int i = 0; i < fop->pre.size; ++i){
        if (pddlFDRPartStateIsSet(&fop->pre, i)
                && !pddlFDRPartStateIsSet(&fop->eff, i)){
            sasCondAdd(&op->prevail, i, fop->pre.val[i]);
        }else if (pddlFDRPartStateIsSet(&fop->eff, i)){
            if (pddlFDRPartStateIsSet(&fop->pre, i)){
                sasOpAddEff(op, i, fop->pre.val[i], fop->eff.val[i]);
            }else{
                sasOpAddEff(op, i, -1, fop->eff.val[i]);
            }
        }
    }

    for (int i = 0; i < fop->cond_eff_size; ++i){
        pddl_fdr_cond_eff_t *ce = fop->cond_eff + i;
        for (int j = 0; j < ce->eff.size; ++j){
            if (pddlFDRPartStateIsSet(&ce->eff, j)){
                e = sasOpAddEff(op, j, -1, ce->eff.val[j]);
                for (int k = 0; k < ce->pre.size; ++k){
                    if (pddlFDRPartStateIsSet(&ce->pre, k))
                        sasCondAdd(&e->cond, k, ce->pre.val[k]);
                }
            }
        }
    }
}

static void sasOpFree(sas_op_t *op)
{
    if (op->prevail.var != NULL)
        BOR_FREE(op->prevail.var);
    if (op->prevail.val != NULL)
        BOR_FREE(op->prevail.val);

    for (int i = 0; i < op->eff_size; ++i){
        if (op->eff[i].cond.var != NULL)
            BOR_FREE(op->eff[i].cond.var);
        if (op->eff[i].cond.val != NULL)
            BOR_FREE(op->eff[i].cond.val);
    }
    if (op->eff != NULL)
        BOR_FREE(op->eff);
}

static void printOp(const pddl_fdr_t *fdr,
                    const pddl_fdr_op_t *op, FILE *fout)
{
    sas_op_t sop;

    fprintf(fout, "begin_operator\n");
    fprintf(fout, "%s\n", op->name);

    sasOpInit(&sop, op);
    fprintf(fout, "%d\n", sop.prevail.size);
    for (int i = 0; i < sop.prevail.size; ++i)
        fprintf(fout, "%d %d\n", sop.prevail.var[i], sop.prevail.val[i]);

    fprintf(fout, "%d\n", sop.eff_size);
    for (int i = 0; i < sop.eff_size; ++i){
        sas_eff_t *e = sop.eff + i;
        fprintf(fout, "%d", e->cond.size);
        for (int j = 0; j < e->cond.size; ++j)
            fprintf(fout, " %d %d", e->cond.var[j], e->cond.val[j]);
        fprintf(fout, " %d %d %d\n", e->var, e->pre, e->eff);
    }
    sasOpFree(&sop);

    fprintf(fout, "%d\n", op->cost);
    fprintf(fout, "end_operator\n");
}

void pddlFDRPrintAsFD(const pddl_fdr_t *fdr, FILE *fout)
{
    fprintf(fout, "begin_version\n3\nend_version\n");
    fprintf(fout, "begin_metric\n%d\nend_metric\n", hasMetric(fdr));

    // Variables
    fprintf(fout, "%d\n", fdr->var.size);
    for (int i = 0; i < fdr->var.size; ++i)
        printVar(fdr, i, fdr->var.var + i, fout);

    // TODO: mutex groups
    fprintf(fout, "0\n");

    // Initial state
    fprintf(fout, "begin_state\n");
    for (int i = 0; i < fdr->var.size; ++i)
        fprintf(fout, "%d\n", fdr->init.val[i]);
    fprintf(fout, "end_state\n");

    // Goal
    fprintf(fout, "begin_goal\n");
    printPartState(&fdr->goal, fout);
    fprintf(fout, "end_goal\n");

    // Operators
    fprintf(fout, "%d\n", fdr->op_size);
    for (int i = 0; i < fdr->op_size; ++i)
        printOp(fdr, fdr->op + i, fout);

    // axioms
    fprintf(fout, "0\n");

    // TODO: sg
}
