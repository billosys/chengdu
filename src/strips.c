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

#include <boruvka/alloc.h>
#include "pddl/strips.h"

static char *groundOpName(const pddl_t *pddl,
                          const pddl_action_t *action,
                          const int *args)
{
    int i, slen;
    char *name, *cur;

    slen = strlen(action->name) + 2 + 1;
    for (i = 0; i < action->param.size; ++i)
        slen += 1 + strlen(pddl->obj.obj[args[i]].name);

    cur = name = BOR_ALLOC_ARR(char, slen);
    cur += sprintf(cur, "(%s", action->name);
    for (i = 0; i < action->param.size; ++i)
        cur += sprintf(cur, " %s", pddl->obj.obj[args[i]].name);
    cur += sprintf(cur, ")");

    return name;
}


/**** GROUND FULL ****/
static void groundFullOpRec(pddl_strips_t *strips,
                            const pddl_action_t *action,
                            int *args, int argi)
{
    const int *objs;
    int size, i;

    if (action->param.size == argi){
        pddl_strips_op_t op;
        int ret;

        pddlStripsOpInit(&op);
        ret = pddlCondGroundPre(action->pre, args, &strips->fact, &op.pre, 0);
        ret |= pddlCondGroundEff(action->eff, args,
                                 &strips->fact,
                                 (pddl_facts_t *)&strips->pddl->init_func,
                                 &op.add_eff, &op.del_eff, &op.cost);
        if (ret == 0){
            op.name = groundOpName(strips->pddl, action, args);
            pddlStripsOpsAdd(&strips->op, &op);
        }
        pddlStripsOpFree(&op);
        // TODO: report error if ret != 0

    }else{
        objs = pddlTypesObjsByType(&strips->pddl->type,
                                   action->param.param[argi].type,
                                   &size);
        for (i = 0; i < size; ++i){
            args[argi] = objs[i];
            groundFullOpRec(strips, action, args, argi + 1);
        }
    }
}

static void groundFullOp(pddl_strips_t *strips,
                         const pddl_action_t *action)
{
    int args[action->param.size];
    groundFullOpRec(strips, action, args, 0);
}


static void groundFullPredRec(pddl_strips_t *strips, int pred_id,
                              int *arg, int argi)
{
    const pddl_pred_t *pred = strips->pddl->pred.pred + pred_id;
    int i, obj_size;
    const int *obj;

    if (argi == pred->param_size){
        pddl_fact_t fact;
        pddlFactInit(&fact);
        fact.pred = pred_id;
        fact.arg = arg;
        fact.arg_size = argi;
        pddlFactsAdd(&strips->fact, &fact);

    }else{
        obj = pddlTypesObjsByType(&strips->pddl->type, pred->param[argi],
                                  &obj_size);
        for (i = 0; i < obj_size; ++i){
            arg[argi] = obj[i];
            groundFullPredRec(strips, pred_id, arg, argi + 1);
        }
    }
}

static void groundFullPred(pddl_strips_t *strips, int pred_id)
{
    const pddl_pred_t *pred = strips->pddl->pred.pred + pred_id;
    int arg[pred->param_size];
    groundFullPredRec(strips, pred_id, arg, 0);
}

static void groundFullFacts(pddl_strips_t *strips)
{
    int i;

    for (i = 0; i < strips->pddl->pred.size; ++i)
        groundFullPred(strips, i);
}

static int pddlStripsGroundFull(pddl_strips_t *strips, unsigned flags)
{
    int i;

    groundFullFacts(strips);
    for (i = 0; i < strips->pddl->action.size; ++i)
        groundFullOp(strips, strips->pddl->action.action + i);
    return 0;
}
/**** GROUND FULL END ****/

pddl_strips_t *pddlStripsGround(const pddl_t *pddl, unsigned flags)
{
    pddl_strips_t *strips;

    strips = BOR_ALLOC(pddl_strips_t);
    bzero(strips, sizeof(*strips));
    strips->pddl = pddl;
    pddlFactsInit(&strips->fact);
    pddlStripsOpsInit(&strips->op);
    pddlFactIdArrInit(&strips->init);
    pddlFactIdArrInit(&strips->goal);

    // TODO
    pddlStripsGroundFull(strips, flags);

    return strips;
}

void pddlStripsDel(pddl_strips_t *strips)
{
    pddlFactsFree(&strips->fact);
    pddlStripsOpsFree(&strips->op);
    pddlFactIdArrFree(&strips->init);
    pddlFactIdArrFree(&strips->goal);
    BOR_FREE(strips);
}

void pddlStripsDump(const pddl_strips_t *strips, FILE *fout)
{
    int i, j;

    fprintf(fout, "Fact[%d]:\n", strips->fact.fact_size);
    for (i = 0; i < strips->fact.fact_size; ++i){
        fprintf(fout, "% 4d: ", i);
        pddlFactPrint(strips->pddl, strips->fact.fact + i, fout);
        fprintf(fout, "\n");
    }

    fprintf(fout, "Op[%d]:\n", strips->op.op_size);
    for (i = 0; i < strips->op.op_size; ++i){
        fprintf(fout, "  %s, cost: %d",
                strips->op.op[i].name,
                strips->op.op[i].cost);
        fprintf(fout, ", pre:");
        for (j = 0; j < strips->op.op[i].pre.size; ++j)
            fprintf(fout, " %d", strips->op.op[i].pre.fact[j]);
        fprintf(fout, ", add:");
        for (j = 0; j < strips->op.op[i].add_eff.size; ++j)
            fprintf(fout, " %d", strips->op.op[i].add_eff.fact[j]);
        fprintf(fout, ", del:");
        for (j = 0; j < strips->op.op[i].del_eff.size; ++j)
            fprintf(fout, " %d", strips->op.op[i].del_eff.fact[j]);
        fprintf(fout, "\n");
    }
    // TODO: facts, init, goal
}
