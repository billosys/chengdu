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

    const pddl_action_t *action;
    const pddl_cond_t *pre;
    const pddl_cond_t *eff;
    int *args;
    pddl_strips_op_t op[2];
    int op_id;
    int failed;
};
typedef struct ground_naive ground_naive_t;

static int groundNaivePre(const pddl_cond_atom_t *atom,
                          const pddl_fact_t *fact,
                          void *ud)
{
    ground_naive_t *g = ud;
    int fact_id;

    // TODO: do this somehow better
    // Equality predicate
    if (atom->pred == g->strips->pddl->pred.eq_pred){
        if (atom->neg){
            if (fact->arg[0] == fact->arg[1]){
                g->failed = 1;
                return -2;
            }
        }else{
            if (fact->arg[0] != fact->arg[1]){
                g->failed = 1;
                return -2;
            }
        }
        return 0;
    }

    fact_id = pddlFactsFind(&g->fact, fact);
    if (fact_id >= 0 && factInfo(&g->fact_info, fact_id)->reachable){
        if (atom->neg){
            // negative precondition on a static predicate failed
            g->failed = 1;
            return -2;
        }

        factInfo(&g->fact_info, fact_id)->pre++;
        pddlStripsOpAddPre(&g->op[g->op_id], fact_id);

    }else if (fact_id >= 0 && atom->neg){
        // This corresponds to a negative precondition on a static
        // predicate succeeding
        return 0;

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
    pddlStripsOpAddAddEff(&g->op[g->op_id], fact_id);
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
    pddlStripsOpAddDelEff(&g->op[g->op_id], fact_id);
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
        g->op[g->op_id].cost
                += g->strips->pddl->init_func.fact[func_id]->func_val;
    }else{
        g->op[g->op_id].cost += value;
    }

    return 0;
}

static int groundNaiveGroundOpArgs(ground_naive_t *g, int op_id);

static int canMergeCondEff(const ground_naive_t *g)
{
    const pddl_fact_id_arr_t *pre = &g->op[1].pre;
    int i;

    for (i = 0; i < pre->size; ++i){
        if (!pddlFactIsStatic(g->strips->pddl, g->fact.fact[pre->fact[i]]))
            return 0;
    }
    return 1;
}

static void mergeCondEff(ground_naive_t *g)
{
    int i;

    for (i = 0; i < g->op[1].add_eff.size; ++i){
        pddlStripsOpAddAddEff(&g->op[0], g->op[1].add_eff.fact[i]);
    }
    for (i = 0; i < g->op[1].del_eff.size; ++i)
        pddlStripsOpAddDelEff(&g->op[0], g->op[1].del_eff.fact[i]);
}

static int groundNaiveWhen(const pddl_cond_when_t *when, void *ud)
{
    ground_naive_t *g = ud;
    const pddl_cond_t *pre, *eff;
    int ret = 0;

    if (g->op_id == 1)
        return -4;

    pddlStripsOpInit(&g->op[1]);
    pre = g->pre;
    eff = g->eff;
    g->pre = when->pre;
    g->eff = when->eff;
    if (groundNaiveGroundOpArgs(g, 1) == 0){
        if (canMergeCondEff(g)){
            mergeCondEff(g);
        }else{
            fprintf(stderr, "Skipping (when ) for now...\n");
            fflush(stderr);
            ret = -1;
        }
    }
    pddlStripsOpFree(&g->op[1]);

    g->op_id = 0;
    g->pre = pre;
    g->eff = eff;
    return ret;
}

static int groundNaiveGroundOpArgs(ground_naive_t *g, int op_id)
{
    const pddl_t *pddl = g->strips->pddl;
    char *name;
    int ret;

    g->failed = 0;
    g->op_id = op_id;
    ret = pddlCondGroundPre(pddl, g->pre, g->args, groundNaivePre, g);
    if (ret == -1){
        ERR("Could not ground op `%s' -- precondition is not flattened"
            " conjuction", g->action->name);
        return -1;
    }else if (ret != 0){
        return -2;
    }

    ret = pddlCondGroundEff(pddl, g->eff, g->args,
                            groundNaiveAddEff,
                            groundNaiveDelEff,
                            groundNaiveAssign,
                            groundNaiveWhen,
                            g);
    if (ret == -1){
        ERR("Could not ground op `%s' -- effect is not normalized.",
            g->action->name);
        return -1;
    }else if (ret == -3){
        ERR("Could not ground op `%s' -- unkown function value.",
            g->action->name);
        return -1;
    }else if (ret == -4){
        ERR("Nested conditional effects are not supported (%s).",
            g->action->name);
        return -1;
    }else if (ret != 0){
        return -2;
    }

    name = groundOpName(pddl, g->action, g->args);
    if (pddlStripsOpFinalize(&g->op[g->op_id], name) != 0)
        return -1;
    return 0;
}

static void groundNaiveOpArgs(ground_naive_t *g)
{
    pddlStripsOpInit(&g->op[0]);
    if (groundNaiveGroundOpArgs(g, 0) == 0)
        pddlStripsOpsAdd(&g->strips->op, &g->op[g->op_id]);
    pddlStripsOpFree(&g->op[g->op_id]);
}

static void groundNaiveOpRec(ground_naive_t *g, int argi)
{
    const int *objs;
    int size, i;

    if (g->action->param.size == argi){
        groundNaiveOpArgs(g);
        return;
    }

    objs = pddlTypesObjsByType(&g->strips->pddl->type,
                               g->action->param.param[argi].type, &size);
    for (i = 0; i < size; ++i){
        g->args[argi] = objs[i];
        groundNaiveOpRec(g, argi + 1);
    }
}

static void groundNaiveOp(ground_naive_t *g, const pddl_action_t *action)
{
    int args[action->param.size];
    g->action = action;
    g->pre = action->pre;
    g->eff = action->eff;
    g->args = args;
    groundNaiveOpRec(g, 0);
}

static void groundNaiveOps(ground_naive_t *g)
{
    const pddl_actions_t *as = &g->strips->pddl->action;
    int i;

    for (i = 0; i < as->size; ++i)
        groundNaiveOp(g, as->action + i);
}

static void groundNaiveSetCostToOne(ground_naive_t *g)
{
    pddl_strips_op_t *op;

    PDDL_STRIPS_OPS_FOR_EACH(&g->strips->op, op)
        op->cost = 1;
}

static void groundNaiveRmStaticAndUnreachable(ground_naive_t *g)
{
    const pddl_fact_t *fact;
    const fact_info_t *fi;
    int rm;

    PDDL_FACTS_FOR_EACH(&g->fact, fact){
        fi = factInfo(&g->fact_info, fact->id);
        rm = 0;

        // All static facts can be removed from operators.
        if (fi->add == 0 && fi->del == 0 && fi->reachable){
            rm = 1;
            pddlStripsOpsRmFactId(&g->strips->op, fact->id);
        }

        // If the fact is not reachable but was created as a delete effect,
        // we can safely remove this fact from delete effects.
        if (!fi->reachable && fi->del > 0){
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

    // Set costs to 1 if metric is not defined
    if (!strips->pddl->metric)
        groundNaiveSetCostToOne(&g);

    groundNaiveRmStaticAndUnreachable(&g);
    // TODO: Remove operators without effects
    // TODO: Merge conditional effects without preconditions

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

    // TODO: remove static facts
    // TODO: remove identical operators (don't forget to keep the one with
    // the minimal cost)
    // TODO: causal graph
    // TODO: pruning
    // TODO: is goal reachable?
    // TODO: Compile away conditional effects if set in flags

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
        //fprintf(fout, ", hash: %lu", (long)op->hash);
        fprintf(fout, "\n");
    }
    // TODO: facts, init, goal
}
