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

struct pddl_cond_arr {
    const pddl_cond_t **cond;
    int size;
    int alloc;
};
typedef struct pddl_cond_arr pddl_cond_arr_t;

_bor_inline void pddlCondArrFree(pddl_cond_arr_t *ca)
{
    if (ca->cond)
        BOR_FREE(ca->cond);
}

_bor_inline void pddlCondArrAdd(pddl_cond_arr_t *ca, const pddl_cond_t *c)
{
    if (ca->size >= ca->alloc){
        if (ca->alloc == 0)
            ca->alloc = 1;
        ca->alloc *= 2;
        ca->cond = BOR_REALLOC_ARR(ca->cond, const pddl_cond_t *, ca->alloc);
    }
    ca->cond[ca->size++] = c;
}

struct prep_action {
    const pddl_action_t *action;
    pddl_cond_arr_t pre_eq;
    pddl_cond_arr_t pre_neg;
    pddl_cond_arr_t pre;
    pddl_cond_arr_t add_eff;
    pddl_cond_arr_t del_eff;
    pddl_cond_arr_t assign;
    int max_arg_size;

    struct prep_action *cond_eff;
    int cond_eff_size;
    int cond_eff_alloc;
};
typedef struct prep_action prep_action_t;

struct prep_action_ctx {
    prep_action_t *a;
    const pddl_t *pddl;
    const pddl_action_t *action;
    int failed;
};
typedef struct prep_action_ctx prep_action_ctx_t;

static void prepActionInit(prep_action_t *a,
                           const pddl_t *pddl, const pddl_action_t *action);

static int prepActionInitPre(pddl_cond_t *c, void *ud)
{
    prep_action_ctx_t *ctx = ud;
    pddl_cond_atom_t *a;

    if (c->type == PDDL_COND_ATOM){
        a = PDDL_COND_CAST(c, atom);
        ctx->a->max_arg_size = BOR_MAX(ctx->a->max_arg_size, a->arg_size);
        if (a->pred == ctx->pddl->pred.eq_pred){
            pddlCondArrAdd(&ctx->a->pre_eq, c);
        }else if (a->neg){
            pddlCondArrAdd(&ctx->a->pre_neg, c);
        }else{
            pddlCondArrAdd(&ctx->a->pre, c);
        }
        return 0;

    }else if (c->type == PDDL_COND_AND){
        return 0;
    }else{
        ctx->failed = 1;
        return -2;
    }
}

static int prepActionInitEff(pddl_cond_t *c, void *ud)
{
    prep_action_ctx_t *ctx = ud;
    pddl_cond_atom_t *a;

    if (c->type == PDDL_COND_ATOM){
        a = PDDL_COND_CAST(c, atom);
        ctx->a->max_arg_size = BOR_MAX(ctx->a->max_arg_size, a->arg_size);
        if (a->neg){
            pddlCondArrAdd(&ctx->a->del_eff, c);
        }else{
            pddlCondArrAdd(&ctx->a->add_eff, c);
        }
        return 0;

    }else if (c->type == PDDL_COND_ASSIGN){
        pddlCondArrAdd(&ctx->a->assign, c);
        return 0;

    }else if (c->type == PDDL_COND_WHEN){
        ++ctx->a->cond_eff_size;
        ctx->a->cond_eff = BOR_REALLOC_ARR(ctx->a->cond_eff, prep_action_t,
                                           ctx->a->cond_eff_size);
        prepActionInit(ctx->a->cond_eff + ctx->a->cond_eff_size - 1,
                       ctx->pddl, ctx->action);
        return -1;

    }else if (c->type == PDDL_COND_AND){
        return 0;
    }else{
        ctx->failed = 1;
        return -2;
    }
}

static void prepActionInit(prep_action_t *a,
                           const pddl_t *pddl, const pddl_action_t *action)
{
    prep_action_ctx_t ctx;
    ctx.a = a;
    ctx.pddl = pddl;
    ctx.action = action;
    ctx.failed = 0;

    bzero(a, sizeof(*a));
    a->action = action;
    pddlCondTraverse((pddl_cond_t *)action->pre,
                     prepActionInitPre, NULL, &ctx);
    if (ctx.failed){
        // TODO
        ERR("Prepapration of action %s failed!\n", action->name);
        exit(-1);
    }

    pddlCondTraverse((pddl_cond_t *)action->eff,
                     prepActionInitEff, NULL, &ctx);
    if (ctx.failed){
        // TODO
        ERR("Prepapration of action %s failed!\n", action->name);
        exit(-1);
    }
}

static void prepActionFree(prep_action_t *a)
{
    int i;

    pddlCondArrFree(&a->pre_eq);
    pddlCondArrFree(&a->pre_neg);
    pddlCondArrFree(&a->pre);
    pddlCondArrFree(&a->add_eff);
    pddlCondArrFree(&a->del_eff);
    pddlCondArrFree(&a->assign);

    for (i = 0; i < a->cond_eff_size; ++i)
        prepActionFree(a->cond_eff + i);
}

struct prep_actions {
    prep_action_t *action;
    int size;
};
typedef struct prep_actions prep_actions_t;

static void prepActionsInit(prep_actions_t *as, const pddl_t *pddl)
{
    int i;

    as->size = pddl->action.size;
    as->action = BOR_CALLOC_ARR(prep_action_t, as->size);
    for (i = 0; i < pddl->action.size; ++i)
        prepActionInit(as->action + i, pddl, pddl->action.action + i);
}

static void prepActionsFree(prep_actions_t *as)
{
    int i;
    for (i = 0; i < as->size; ++i)
        prepActionFree(as->action + i);
    if (as->action)
        BOR_FREE(as->action);
}


struct partground_op {
    int action_id;
    int *arg;
    int arg_set;
};
typedef struct partground_op partground_op_t;

static void partgroundOpInit(partground_op_t *op,
                             int a_id, prep_action_t *a)
{
    int i;

    op->action_id = a_id;
    op->arg = BOR_ALLOC_ARR(int, a->action->param.size);
    for (i = 0; i < a->action->param.size; ++i)
        op->arg[i] = -1;
    op->arg_set = 0;
}

static void partgroundOpFree(partground_op_t *op)
{
    if (op->arg)
        BOR_FREE(op->arg);
}

static void partgroundOpSet(partground_op_t *op,
                            const int *arg, int arg_size,
                            int arg_set)
{
    memcpy(op->arg, arg, sizeof(int) * arg_size);
    op->arg_set = arg_set;
}

struct partground_ops {
    partground_op_t *op;
    int size;
    int alloc;
    // TODO: htable
};
typedef struct partground_ops partground_ops_t;

static int partgroundOpsAdd(partground_ops_t *ops, int a_id, prep_action_t *a);
static void partgroundOpsInit(partground_ops_t *ops, prep_actions_t *pa)
{
    int i;

    bzero(ops, sizeof(*ops));
    for (i = 0; i < pa->size; ++i)
        partgroundOpsAdd(ops, i, pa->action + i);
}

static void partgroundOpsFree(partground_ops_t *ops)
{
    int i;
    for (i = 0; i < ops->size; ++i)
        partgroundOpFree(ops->op + i);
    if (ops->op)
        BOR_FREE(ops->op);
}

static int partgroundOpsAdd(partground_ops_t *ops, int a_id, prep_action_t *a)
{
    partground_op_t *pop;

    if (ops->size >= ops->alloc){
        if (ops->alloc == 0)
            ops->alloc = 16;
        ops->alloc *= 2;
        ops->op = BOR_REALLOC_ARR(ops->op, partground_op_t, ops->alloc);
    }

    pop = ops->op + ops->size++;
    partgroundOpInit(pop, a_id, a);
    return ops->size - 1;
}


struct pred_to_partground_op {
    int op_id;
    int pre_id;
};
typedef struct pred_to_partground_op pred_to_partground_op_t;

struct pred_to_partground_ops {
    pred_to_partground_op_t *op;
    int size;
    int alloc;
};
typedef struct pred_to_partground_ops pred_to_partground_ops_t;

static void predToPartgroundOpsInit(pred_to_partground_ops_t *ops)
{
    bzero(ops, sizeof(*ops));
}

static void predToPartgroundOpsFree(pred_to_partground_ops_t *ops)
{
    if (ops->op)
        BOR_FREE(ops->op);
}

static void predToPartgroundOpsAdd(pred_to_partground_ops_t *ops,
                                   int op_id, int pre_id)
{
    pred_to_partground_op_t *pop;

    if (ops->size >= ops->alloc){
        if (ops->alloc == 0)
            ops->alloc = 2;
        ops->alloc *= 2;
        ops->op = BOR_REALLOC_ARR(ops->op, pred_to_partground_op_t,
                                  ops->alloc);
    }

    pop = ops->op + ops->size++;
    pop->op_id = op_id;
    pop->pre_id = pre_id;
    fprintf(stderr, "predAdd: op_id: %d, pre_id: %d\n", op_id, pre_id);
}

struct pred_to_partground_op_table {
    pred_to_partground_ops_t *pred;
    int size;
};
typedef struct pred_to_partground_op_table pred_to_partground_op_table_t;

static void predToPartgroundOpTableInit(pred_to_partground_op_table_t *t,
                                        const pddl_t *pddl)
{
    int i;

    t->size = pddl->pred.size;
    t->pred = BOR_ALLOC_ARR(pred_to_partground_ops_t, t->size);
    for (i = 0; i < t->size; ++i)
        predToPartgroundOpsInit(t->pred + i);
}

static void predToPartgroundOpTableFree(pred_to_partground_op_table_t *t)
{
    int i;
    for (i = 0; i < t->size; ++i)
        predToPartgroundOpsFree(t->pred + i);
    if (t->pred)
        BOR_FREE(t->pred);
}

struct ground {
    const pddl_t *pddl;
    pddl_facts_t static_fact;
    pddl_facts_t fact;
    pddl_strips_ops_t op;

    prep_actions_t prep_action;
    partground_ops_t partground_op;
    pred_to_partground_op_table_t pred_table;
};
typedef struct ground ground_t;

static void groundInsertInitPartgroundOps(ground_t *g)
{
    partground_op_t *pop;
    prep_action_t *pa;
    pred_to_partground_ops_t *pred_to_pop;
    const pddl_cond_atom_t *atom;
    int i, j;

    for (i = 0; i < g->partground_op.size; ++i){
        pop = g->partground_op.op + i;
        pa = g->prep_action.action + pop->action_id;
        for (j = 0; j < pa->pre.size; ++j){
            atom = PDDL_COND_CAST(pa->pre.cond[j], atom);
            pred_to_pop = g->pred_table.pred + atom->pred;
            predToPartgroundOpsAdd(pred_to_pop, i, j);
        }
    }

    // TODO: Conditional effects
}

static void groundInit(ground_t *g, const pddl_t *pddl)
{
    pddl_fact_t *f;

    g->pddl = pddl;
    pddlFactsInit(&g->static_fact);
    pddlFactsInit(&g->fact);
    pddlStripsOpsInit(&g->op);
    prepActionsInit(&g->prep_action, pddl);
    partgroundOpsInit(&g->partground_op, &g->prep_action);
    predToPartgroundOpTableInit(&g->pred_table, pddl);
    groundInsertInitPartgroundOps(g);

    PDDL_FACTS_FOR_EACH(&pddl->init_fact, f){
        if (pddlFactIsStatic(pddl, f)){
            pddlFactsAdd(&g->static_fact, f);
        }
        pddlFactsAdd(&g->fact, f);
    }
}

static void groundFree(ground_t *g)
{
    pddlFactsFree(&g->static_fact);
    pddlFactsFree(&g->fact);
    pddlStripsOpsFree(&g->op);
    prepActionsFree(&g->prep_action);
    partgroundOpsFree(&g->partground_op);
    predToPartgroundOpTableFree(&g->pred_table);
}

static int prepActionCheckPreEq(const prep_action_t *pa, const int *arg)
{
    const pddl_cond_atom_t *atom;
    int i, o1, o2, eq;

    for (i = 0; i < pa->pre_eq.size; ++i){
        atom = PDDL_COND_CAST(pa->pre_eq.cond[i], atom);
        o1 = (atom->arg[0].obj >= 0 ? atom->arg[0].obj
                    : arg[atom->arg[0].param]);
        o2 = (atom->arg[1].obj >= 0 ? atom->arg[1].obj
                    : arg[atom->arg[1].param]);
        if (o1 == -1 && o2 == -1)
            continue;
        eq = o1 == o2;
        if (atom->neg && eq)
            return -1;
        if (!atom->neg && !eq)
            return -1;
    }

    return 0;
}

static int prepActionCheckPreNeg(const prep_action_t *pa, const int *arg,
                                 const pddl_facts_t *static_facts)
{
    const pddl_cond_atom_t *atom;
    int i, fact_id;
    PDDL_FACT_FOR_GROUND2(fact, pa->max_arg_size);

    for (i = 0; i < pa->pre_neg.size; ++i){
        atom = PDDL_COND_CAST(pa->pre_eq.cond[i], atom);
        if (pddlCondAtomGroundFact(atom, arg, &fact) != 0)
            continue;
        fact_id = pddlFactsFind(static_facts, &fact);
        if (fact_id < 0 && !atom->neg)
            return -1;
        if (fact_id >= 0 && atom->neg)
            return -1;
    }

    return 0;
}

static void partgroundOpGroundEff(ground_t *g, int pop_id)
{
    const partground_op_t *pop = g->partground_op.op + pop_id;
    const prep_action_t *pa = g->prep_action.action + pop->action_id;
    const pddl_cond_atom_t *atom;
    int i;
    PDDL_FACT_FOR_GROUND2(fact, pa->max_arg_size);

    for (i = 0; i < pa->add_eff.size; ++i){
        atom = PDDL_COND_CAST(pa->add_eff.cond[i], atom);
        pddlCondAtomGroundFact(atom, pop->arg, &fact);
        int n = g->fact.fact_size;
        pddlFactsAdd(&g->fact, &fact);
        fprintf(stderr, "ADD ");
        pddlFactPrint(g->pddl, &fact, stderr);
        fprintf(stderr, " :: %d -> %d\n", n, g->fact.fact_size);
    }
}

static void predToPartgroundAdd(ground_t *g, int pop_id)
{
    const partground_op_t *pop = g->partground_op.op + pop_id;
    const prep_action_t *pa = g->prep_action.action + pop->action_id;
    const pddl_cond_atom_t *atom;
    int i, j, param_id;

    for (i = 0; i < pa->pre.size; ++i){
        atom = PDDL_COND_CAST(pa->pre.cond[i], atom);
        for (j = 0; j < atom->arg_size; ++j){
            param_id = atom->arg[j].param;
            if (param_id >= 0 && pop->arg[param_id] == -1){
                predToPartgroundOpsAdd(g->pred_table.pred + atom->pred,
                                       pop_id, i);
                break;
            }
        }
    }
}

static int partgroundOp(ground_t *g,
                        const pddl_fact_t *fact,
                        const partground_op_t *op,
                        int pre_id)
{
    const prep_action_t *pa = g->prep_action.action + op->action_id;
    const pddl_cond_t *cond = pa->pre.cond[pre_id];
    const pddl_cond_atom_t *atom = PDDL_COND_CAST(cond, atom);
    int arg[pa->action->param.size];
    int param_id;
    int i, arg_set = op->arg_set;
    int pop_id;
    partground_op_t *newop;

    memcpy(arg, op->arg, sizeof(int) * pa->action->param.size);
    for (i = 0; i < fact->arg_size; ++i){
        if (atom->arg[i].obj >= 0){
            // Check that objects are equal
            if (atom->arg[i].obj != fact->arg[i])
                return -1;

        }else{
            param_id = atom->arg[i].param;

            // Check that if the operator argument is already set, then the
            // objects are equal.
            if (op->arg[param_id] != -1 && op->arg[param_id] != fact->arg[i])
                return -1;

            if (op->arg[param_id] == -1){
                if (!pddlTypesObjHasType(&g->pddl->type,
                                   pa->action->param.param[param_id].type,
                                   fact->arg[i]))
                    return -1;

                arg[param_id] = fact->arg[i];
                ++arg_set;
            }
        }
    }

    // If no argument is added, we don't need to ground this action
    if (arg_set == op->arg_set)
        return -1;

    // Check equality preconditions
    if (prepActionCheckPreEq(pa, arg) != 0)
        return -1;

    // Check negative static preconditions
    if (prepActionCheckPreNeg(pa, arg, &g->static_fact) != 0)
        return -1;

    // Create a new partially grounded operator
    pop_id = partgroundOpsAdd(&g->partground_op, op->action_id,
                              g->prep_action.action + op->action_id);
    newop = g->partground_op.op + pop_id;
    partgroundOpSet(newop, arg, pa->action->param.size, arg_set);

    if (arg_set == pa->action->param.size){
        // If all arguments are set, ground add effects, but do not add
        // operator to a table
        char *name = groundOpName(g->pddl, pa->action, newop->arg);
        fprintf(stderr, "X: adding: %s\n", name);
        BOR_FREE(name);
        partgroundOpGroundEff(g, pop_id);

    }else{
        predToPartgroundAdd(g, pop_id);
    }

    // TODO: Conditional effects

    return 0;
}

static void groundFact(ground_t *g, pddl_fact_t *fact)
{
    pred_to_partground_ops_t *pred_to_pop;
    int i, op_id, pre_id;

    fprintf(stderr, "groundFact: ");
    pddlFactPrint(g->pddl, fact, stderr);
    fprintf(stderr, "\n");

    pred_to_pop = g->pred_table.pred + fact->pred;
    for (i = 0; i < pred_to_pop->size; ++i){
        op_id = pred_to_pop->op[i].op_id;
        pre_id = pred_to_pop->op[i].pre_id;
        int ret = partgroundOp(g, fact, g->partground_op.op + op_id, pre_id);

        fprintf(stderr, "  ==> args:");
        for (int j = 0; j <
                g->prep_action.action[g->partground_op.op[op_id].action_id].action->param.size;
                ++j){
            fprintf(stderr, " %d", g->partground_op.op[op_id].arg[j]);
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "  ==> op_id: %d, pre_id: %d, ret: %d\n",
                op_id, pre_id, ret);
        fprintf(stderr, "  ==> ");
        pddlActionPrint(g->pddl,
           g->prep_action.action[g->partground_op.op[op_id].action_id].action,
           stderr);
        fprintf(stderr, "\n");
    }
}

static void ground(ground_t *g)
{
    pddl_fact_t *fact;
    int i;

    // TODO: actions without parameters
    fprintf(stderr, "G\n");
    for (i = 0; i < g->fact.fact_size; ++i){
        fact = g->fact.fact[i];
        groundFact(g, fact);
    }
}


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
    /*
    {
        ground_t g;
        groundInit(&g, pddl);
        ground(&g);
        groundFree(&g);
    }
    */

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
