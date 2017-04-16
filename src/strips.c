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
#include "err.h"

struct fact_info {
    int reachable;
    int pre;
    int add;
    int del;
};
typedef struct fact_info fact_info_t;

struct fact_infos {
    fact_info_t *info;
    int size;
};
typedef struct fact_infos fact_infos_t;

_bor_inline void factInfoInit(fact_info_t *fi)
{
    bzero(fi, sizeof(*fi));
}

_bor_inline void factInfoFree(fact_info_t *fi)
{
}

_bor_inline void factInfosInit(fact_infos_t *fi)
{
    bzero(fi, sizeof(*fi));
}

_bor_inline void factInfosFree(fact_infos_t *fi)
{
    if (fi->info)
        BOR_FREE(fi->info);
}

fact_info_t *factInfo(fact_infos_t *fi, int fact_id)
{
    if (fi->size <= fact_id){
        int init_size = fi->size;

        if (fi->size == 0)
            fi->size = 1;
        while (fi->size <= fact_id)
            fi->size *= 2;
        fi->info = BOR_REALLOC_ARR(fi->info, fact_info_t, fi->size);
        for (int i = init_size; i < fi->size; ++i)
            factInfoInit(fi->info + i);
    }

    return fi->info + fact_id;
}


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


/**** GROUND NAIVE ****/
struct ground_naive {
    pddl_strips_t *strips;
    pddl_facts_t fact;
    fact_infos_t fact_info;

    pddl_strips_op_t op;
    int failed;
};
typedef struct ground_naive ground_naive_t;

static int groundNaivePre(const pddl_cond_atom_t *atom,
                          const pddl_fact_t *fact,
                          void *ud)
{
    ground_naive_t *g = ud;
    int fact_id;

    fact_id = pddlFactsFind(&g->fact, fact);
    if (fact_id >= 0 && factInfo(&g->fact_info, fact_id)->reachable){
        factInfo(&g->fact_info, fact_id)->pre++;
        pddlFactIdArrAdd(&g->op.pre, fact_id);
    }else{
        g->failed = 1;
        return -2;
    }
    return 0;
}

static int groundNaiveAddEff(const pddl_cond_atom_t *atom,
                             const pddl_fact_t *fact,
                             void *ud)
{
    ground_naive_t *g = ud;
    fact_info_t *fi;
    int fact_id;

    fact_id = pddlFactsAdd(&g->fact, fact);
    fi = factInfo(&g->fact_info, fact_id);
    fi->reachable = 1;
    ++fi->add;
    pddlFactIdArrAdd(&g->op.add_eff, fact_id);
    return 0;
}

static int groundNaiveDelEff(const pddl_cond_atom_t *atom,
                             const pddl_fact_t *fact,
                             void *ud)
{
    ground_naive_t *g = ud;
    int fact_id;

    fact_id = pddlFactsAdd(&g->fact, fact);
    factInfo(&g->fact_info, fact_id)->del++;
    pddlFactIdArrAdd(&g->op.del_eff, fact_id);
    return 0;
}

static int groundNaiveAssign(const pddl_cond_assign_t *assign,
                             int value,
                             const pddl_fact_t *fvalue,
                             void *ud)
{
    ground_naive_t *g = ud;
    int func_id;

    if (fvalue != NULL){
        func_id = pddlFactsFind(&g->strips->pddl->init_func, fvalue);
        if (func_id < 0)
            return -3;
        g->op.cost += g->strips->pddl->init_func.fact[func_id]->func_val;
    }else{
        g->op.cost += value;
    }

    return 0;
}

static int groundNaiveWhen(const pddl_cond_when_t *when, void *ud)
{
    fprintf(stderr, "Skipping (when ) for now...\n");
    return 0;
}

static void groundNaiveOpArgs(ground_naive_t *g,
                              const pddl_action_t *action,
                              const int *args)
{
    const pddl_t *pddl = g->strips->pddl;
    int ret;

    g->failed = 0;
    pddlStripsOpInit(&g->op);
    ret = pddlCondGroundPre(pddl, action->pre, args, groundNaivePre, g);
    if (ret == -1){
        ERR("Could not ground op `%s' -- precondition is not flattened"
            " conjuction", action->name);
        goto ground_naive_op_args_fail;
    }else if (ret != 0){
        goto ground_naive_op_args_fail;
    }

    ret = pddlCondGroundEff(pddl, action->eff, args,
                            groundNaiveAddEff,
                            groundNaiveDelEff,
                            groundNaiveAssign,
                            groundNaiveWhen,
                            g);
    if (ret == -1){
        ERR("Could not ground op `%s' -- effect is not normalized.",
            action->name);
        goto ground_naive_op_args_fail;
    }else if (ret == -3){
        ERR("Could not ground op `%s' -- unkown function value.",
            action->name);
        goto ground_naive_op_args_fail;
    }else if (ret != 0){
        goto ground_naive_op_args_fail;
    }

    if (pddlStripsOpFinalize(&g->op, groundOpName(pddl, action, args)) != 0)
        goto ground_naive_op_args_fail;

    pddlStripsOpsAdd(&g->strips->op, &g->op);

ground_naive_op_args_fail:
    pddlStripsOpFree(&g->op);
}

static void groundNaiveOpRec(ground_naive_t *g,
                             const pddl_action_t *action,
                             int *args, int argi)
{
    const int *objs;
    int size, i;

    if (action->param.size == argi){
        groundNaiveOpArgs(g, action, args);
        return;
    }

    objs = pddlTypesObjsByType(&g->strips->pddl->type,
                               action->param.param[argi].type, &size);
    for (i = 0; i < size; ++i){
        args[argi] = objs[i];
        groundNaiveOpRec(g, action, args, argi + 1);
    }
}

static void groundNaiveOp(ground_naive_t *g, const pddl_action_t *action)
{
    int args[action->param.size];
    groundNaiveOpRec(g, action, args, 0);
}

static void groundNaiveOps(ground_naive_t *g)
{
    const pddl_actions_t *as = &g->strips->pddl->action;
    int i;

    for (i = 0; i < as->size; ++i)
        groundNaiveOp(g, as->action + i);
}

static void groundNaiveRmStaticAndUnreachable(ground_naive_t *g)
{
    const pddl_fact_t *fact;
    const fact_info_t *fi;
    int rm;

    PDDL_FACTS_FOR_EACH(&g->fact, fact){
        fi = factInfo(&g->fact_info, fact->id);
        rm = 0;

        // If the fact is never added and is reachable, then it is a static
        // fact. Therefore it can be removed all together from all
        // operators.
        if (fi->add == 0 && fi->reachable){
            rm = 1;
            pddlStripsOpsRmFactId(&g->strips->op, fact->id);
        }

        // If the fact is not reachable but was created as a delete effect,
        // we can safely remove this fact from delete effects.
        if (!fi->reachable && fi->del > 0){
            rm = 1;
            pddlStripsOpsRmFactIdFromDelEff(&g->strips->op, fact->id);
        }

        if (rm)
            pddlFactsDelFact(&g->fact, fact->id);
    }
}

static int groundNaive(pddl_strips_t *strips, unsigned flags)
{
    ground_naive_t g;
    int num_ops;

    g.strips = strips;
    pddlFactsInit(&g.fact);
    factInfosInit(&g.fact_info);

    pddlFactsCopy(&g.fact, &strips->pddl->init_fact);
    for (int i = 0; i < g.fact.fact_size; ++i){
        factInfo(&g.fact_info, i)->reachable = 1;
    }

    num_ops = -1;
    while (num_ops != strips->op.op_size){
        num_ops = strips->op.op_size;
        groundNaiveOps(&g);
    }

    groundNaiveRmStaticAndUnreachable(&g);

    factInfosFree(&g.fact_info);
    //pddlFactsFree(&g.fact);
    strips->fact = g.fact;
    return 0;
}
/**** GROUND NAIVE END ****/

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
    groundNaive(strips, flags);

    // TODO: set cost to 1 if necessary
    // TODO: remove static facts
    // TODO: remove identical operators (don't forget to keep the one with
    // the minimal cost)
    // TODO: causal graph
    // TODO: pruning
    // TODO: is goal reachable?
    // TODO: Compile away condition effects

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
    const pddl_strips_op_t *op;
    const pddl_fact_t *fact;
    int j, cnt;

    cnt = 0;
    PDDL_FACTS_FOR_EACH(&strips->fact, fact)
        ++cnt;
    fprintf(fout, "Fact[%d]:\n", cnt);
    PDDL_FACTS_FOR_EACH(&strips->fact, fact){
        fprintf(fout, "% 4d: ", fact->id);
        pddlFactPrint(strips->pddl, fact, fout);
        fprintf(fout, "\n");
    }

    cnt = 0;
    PDDL_STRIPS_OPS_FOR_EACH(&strips->op, op)
        ++cnt;
    fprintf(fout, "Op[%d]:\n", cnt);
    PDDL_STRIPS_OPS_FOR_EACH(&strips->op, op){
        fprintf(fout, "% 4d: %s, cost: %d", op->id, op->name, op->cost);
        fprintf(fout, ", pre:");
        for (j = 0; j < op->pre.size; ++j)
            fprintf(fout, " %d", op->pre.fact[j]);
        fprintf(fout, ", add:");
        for (j = 0; j < op->add_eff.size; ++j)
            fprintf(fout, " %d", op->add_eff.fact[j]);
        fprintf(fout, ", del:");
        for (j = 0; j < op->del_eff.size; ++j)
            fprintf(fout, " %d", op->del_eff.fact[j]);
        fprintf(fout, "\n");
    }
    // TODO: facts, init, goal
}
