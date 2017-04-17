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
#include <boruvka/hfunc.h>
#include "pddl/pddl.h"
#include "pddl/ground_action.h"
#include "err.h"

struct action_ctx {
    pddl_ground_actions_t *as;
    pddl_ground_action_t *a;
    int a_id;
    const pddl_t *pddl;
    const pddl_action_t *action;
    int failed;
};
typedef struct action_ctx action_ctx_t;

static int actionInitPre(pddl_cond_t *c, void *ud)
{
    action_ctx_t *ctx = ud;
    pddl_cond_atom_t *a;

    if (c->type == PDDL_COND_ATOM){
        a = PDDL_COND_CAST(c, atom);
        ctx->a->max_arg_size = BOR_MAX(ctx->a->max_arg_size, a->arg_size);
        if (a->neg){
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

static int actionInitEff(pddl_cond_t *c, void *ud)
{
    action_ctx_t *ctx = ud;
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
        return -1;

    }else if (c->type == PDDL_COND_AND){
        return 0;
    }else{
        ctx->failed = 1;
        return -2;
    }
}

static void actionInit2(pddl_ground_action_t *a,
                        const pddl_t *pddl,
                        const pddl_action_t *action,
                        pddl_cond_t *pre,
                        pddl_cond_t *eff)
{
    action_ctx_t ctx;
    ctx.a = a;
    ctx.pddl = pddl;
    ctx.failed = 0;

    bzero(a, sizeof(*a));
    a->action = action;
    a->param_size = action->param.size;
    pddlCondTraverse(pre, actionInitPre, NULL, &ctx);
    if (ctx.failed){
        // TODO
        ERR("Prepapration of action %s failed!\n", action->name);
        exit(-1);
    }

    pddlCondTraverse(eff, actionInitEff, NULL, &ctx);
    if (ctx.failed){
        // TODO
        ERR("Prepapration of action %s failed!\n", action->name);
        exit(-1);
    }
}

static void actionInit(pddl_ground_action_t *a,
                       const pddl_t *pddl,
                       const pddl_action_t *action)
{
    actionInit2(a, pddl, action,
                (pddl_cond_t *)action->pre,
                (pddl_cond_t *)action->eff);
}

static void actionFree(pddl_ground_action_t *a)
{
    pddlCondArrFree(&a->pre_neg);
    pddlCondArrFree(&a->pre);
    pddlCondArrFree(&a->add_eff);
    pddlCondArrFree(&a->del_eff);
    pddlCondArrFree(&a->assign);
}

static void actionsReserve(pddl_ground_actions_t *as)
{
    if (as->size >= as->alloc){
        as->alloc *= 2;
        as->action = BOR_REALLOC_ARR(as->action, pddl_ground_action_t, as->alloc);
    }
}

static int actionInitCondEff(pddl_cond_t *c, void *ud)
{
    action_ctx_t *ctx = ud;
    const pddl_cond_when_t *when;
    pddl_ground_action_t *a, *parent;

    if (c->type == PDDL_COND_WHEN){
        when = PDDL_COND_CAST(c, when);

        // Create a new action
        actionsReserve(ctx->as);
        a = ctx->as->action + ctx->as->size++;

        // Parse preconditions and effects of (when ) element
        actionInit2(a, ctx->pddl, ctx->action, when->pre, when->eff);
        if (a->cond_eff_size > 0){
            ERR2("Nested conditional effects are not supported!");
            exit(-1);
        }

        // Set its parent
        parent = ctx->as->action + ctx->a_id;
        a->parent_action = ctx->a_id;

        // Copy preconditions
        for (int i = 0; i < parent->pre_neg.size; ++i)
            pddlCondArrAdd(&a->pre_neg, parent->pre_neg.cond[i]);
        for (int i = 0; i < parent->pre.size; ++i)
            pddlCondArrAdd(&a->pre, parent->pre.cond[i]);
        a->max_arg_size = BOR_MAX(a->max_arg_size, parent->max_arg_size);


        return -1;
    }
    return 0;
}

static void actionsAddCondEff(pddl_ground_actions_t *as, int aid, const pddl_t *pddl)
{
    action_ctx_t ctx;
    ctx.as = as;
    ctx.a_id = aid;
    ctx.pddl = pddl;
    ctx.action = as->action[aid].action;
    ctx.failed = 0;

    pddlCondTraverse((pddl_cond_t *)ctx.action->eff,
                     actionInitCondEff, NULL, &ctx);
    if (ctx.failed){
        // TODO
        ERR("Prepapration of action %s failed!\n", ctx.action->name);
        exit(-1);
    }
}

void pddlGroundActionsInit(const pddl_t *pddl, pddl_ground_actions_t *as)
{
    int i;

    bzero(as, sizeof(*as));
    as->alloc = 4;
    as->action = BOR_ALLOC_ARR(pddl_ground_action_t, as->alloc);

    for (i = 0; i < pddl->action.size; ++i){
        actionsReserve(as);
        actionInit(as->action + as->size, pddl, pddl->action.action + i);
        ++as->size;
    }

    for (i = 0; i < pddl->action.size; ++i){
        if (as->action[i].cond_eff_size > 0)
            actionsAddCondEff(as, i, pddl);
    }
}

void pddlGroundActionsFree(pddl_ground_actions_t *as)
{
    for (int i = 0; i < as->size; ++i)
        actionFree(as->action + i);
    if (as->action)
        BOR_FREE(as->action);
}
