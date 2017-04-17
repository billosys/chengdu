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
#include <boruvka/htable.h>
#include <boruvka/hfunc.h>
#include "pddl/strips.h"
#include "pddl/cond.h"
#include "err.h"

struct cond_arr {
    const pddl_cond_t **cond;
    int size;
    int alloc;
};
typedef struct cond_arr cond_arr_t;

struct action {
    const pddl_action_t *action;
    // TODO
    int parent_action; /*!< ID >= 0 if this conditional effect */
    int param_size;
    cond_arr_t pre_neg;
    cond_arr_t pre;
    cond_arr_t add_eff;
    cond_arr_t del_eff;
    cond_arr_t assign;
    int max_arg_size;
    int cond_eff_size;
};
typedef struct action action_t;

struct actions {
    action_t *action;
    int size;
    int alloc;
};
typedef struct actions actions_t;

struct ground_atom {
    bor_list_t htable;
    int id;

    int pred;
    int arg_size;
    pddl_cond_atom_arg_t arg[];
};
typedef struct ground_atom ground_atom_t;

struct ground_atoms {
    ground_atom_t **atom;
    int size;
    int alloc;
    bor_htable_t *htable;
};
typedef struct ground_atoms ground_atoms_t;

struct ground_action {
    bor_list_t htable;
    int id;

    int action_id;
    int arg_size;
    int ground_atom_size;
    int data[];
};
typedef struct ground_action ground_action_t;

#define GROUND_ACTION_GROUND_ATOM(GA) \
    ((GA)->data + 0)
#define GROUND_ACTION_ARG(GA) \
    ((GA)->data + (GA)->ground_atom_size)

struct ground_actions {
    ground_action_t **action;
    int size;
    int alloc;
    bor_htable_t *htable;
};
typedef struct ground_actions ground_actions_t;

struct pred_map {
    int **map_ground_action;
    int **map_pre_id;
    int *map_size;
    int *map_alloc;
    int pred_size;
};
typedef struct pred_map pred_map_t;

struct ground {
    const pddl_t *pddl;
    actions_t action;
    ground_atoms_t ground_atom;
    ground_actions_t ground_action;
    pred_map_t pred_map;

    pddl_facts_t static_fact;
    pddl_facts_t fact;
    pddl_strips_ops_t op;
};
typedef struct ground ground_t;

static void groundActionPrint(ground_t *g, const ground_action_t *ga,
                              FILE *fout);

static void condArrFree(cond_arr_t *ca)
{
    if (ca->cond)
        BOR_FREE(ca->cond);
}

static void condArrAdd(cond_arr_t *ca, const pddl_cond_t *c)
{
    if (ca->size >= ca->alloc){
        if (ca->alloc == 0)
            ca->alloc = 1;
        ca->alloc *= 2;
        ca->cond = BOR_REALLOC_ARR(ca->cond, const pddl_cond_t *, ca->alloc);
    }
    ca->cond[ca->size++] = c;
}

static void condArrCopy(cond_arr_t *dst, const cond_arr_t *src)
{
    *dst = *src;
    if (src->cond != NULL){
        dst->cond = BOR_ALLOC_ARR(const pddl_cond_t *, dst->alloc);
        memcpy(dst->cond, src->cond, sizeof(pddl_cond_t *) * src->size);
    }
}

struct action_ctx {
    actions_t *as;
    action_t *a;
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
            condArrAdd(&ctx->a->pre_neg, c);
        }else{
            condArrAdd(&ctx->a->pre, c);
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
            condArrAdd(&ctx->a->del_eff, c);
        }else{
            condArrAdd(&ctx->a->add_eff, c);
        }
        return 0;

    }else if (c->type == PDDL_COND_ASSIGN){
        condArrAdd(&ctx->a->assign, c);
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

static void actionInit2(action_t *a,
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

static void actionInit(action_t *a,
                       const pddl_t *pddl, const pddl_action_t *action)
{
    actionInit2(a, pddl, action,
                (pddl_cond_t *)action->pre,
                (pddl_cond_t *)action->eff);
}

static void actionFree(action_t *a)
{
    condArrFree(&a->pre_neg);
    condArrFree(&a->pre);
    condArrFree(&a->add_eff);
    condArrFree(&a->del_eff);
    condArrFree(&a->assign);
}

static void actionCopy(action_t *dst, const action_t *src)
{
    dst->action = src->action;
    dst->parent_action = src->parent_action;
    condArrCopy(&dst->pre_neg, &src->pre_neg);
    condArrCopy(&dst->pre, &src->pre);
    condArrCopy(&dst->add_eff, &src->add_eff);
    condArrCopy(&dst->del_eff, &src->del_eff);
    condArrCopy(&dst->assign, &src->assign);
    dst->max_arg_size = src->max_arg_size;
    dst->cond_eff_size = src->cond_eff_size;
}

static void actionsReserve(actions_t *as)
{
    if (as->size >= as->alloc){
        as->alloc *= 2;
        as->action = BOR_REALLOC_ARR(as->action, action_t, as->alloc);
    }
}

static int actionInitCondEff(pddl_cond_t *c, void *ud)
{
    action_ctx_t *ctx = ud;
    const pddl_cond_when_t *when;
    action_t *a, *parent;

    if (c->type == PDDL_COND_WHEN){
        when = PDDL_COND_CAST(c, when);
        fprintf(stdout, "X\n");
        fflush(stdout);

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
            condArrAdd(&a->pre_neg, parent->pre_neg.cond[i]);
        for (int i = 0; i < parent->pre.size; ++i)
            condArrAdd(&a->pre, parent->pre.cond[i]);
        a->max_arg_size = BOR_MAX(a->max_arg_size, parent->max_arg_size);


        return -1;
    }
    return 0;
}

static void actionsAddCondEff(actions_t *as, int aid, const pddl_t *pddl)
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

static void actionsInit(actions_t *as, const pddl_t *pddl)
{
    int i;

    bzero(as, sizeof(*as));
    as->alloc = 4;
    as->action = BOR_ALLOC_ARR(action_t, as->alloc);

    for (i = 0; i < pddl->action.size; ++i){
        actionsReserve(as);
        actionInit(as->action + as->size, pddl, pddl->action.action + i);
        ++as->size;
    }

    for (i = 0; i < pddl->action.size; ++i){
        if (as->action[i].cond_eff_size > 0){
            fprintf(stdout, "CE %s\n", as->action[i].action->name);
            actionsAddCondEff(as, i, pddl);
        }
    }
}

static void actionsFree(actions_t *as)
{
    for (int i = 0; i < as->size; ++i)
        actionFree(as->action + i);
    if (as->action)
        BOR_FREE(as->action);
}

static ground_atom_t *groundAtomFromAtom(const pddl_cond_atom_t *atom)
{
    ground_atom_t *ga;
    size_t alloc;

    alloc  = bor_offsetof(ground_atom_t, arg);
    alloc += sizeof(pddl_cond_atom_arg_t) * atom->arg_size;
    ga = BOR_MALLOC(alloc);
    ga->pred = atom->pred;
    ga->arg_size = atom->arg_size;
    memcpy(ga->arg, atom->arg, sizeof(pddl_cond_atom_arg_t) * ga->arg_size);
    return ga;
}

static ground_atom_t *groundAtomUnify(const ground_atom_t *from,
                                      const int *arg)
{
    ground_atom_t *ga;
    size_t alloc;
    int param_id;

    alloc  = bor_offsetof(ground_atom_t, arg);
    alloc += sizeof(pddl_cond_atom_arg_t) * from->arg_size;
    ga = BOR_MALLOC(alloc);
    ga->pred = from->pred;
    ga->arg_size = from->arg_size;
    memcpy(ga->arg, from->arg, sizeof(pddl_cond_atom_arg_t) * ga->arg_size);
    for (int i = 0; i < ga->arg_size; ++i){
        param_id = ga->arg[i].param;
        if (param_id >= 0 && arg[param_id] >= 0){
            ga->arg[i].param = -1;
            ga->arg[i].obj = arg[param_id];
        }
    }
    return ga;
}

static void groundAtomDel(ground_atom_t *ga)
{
    BOR_FREE(ga);
}

static bor_htable_key_t groundAtomHash(const bor_list_t *key, void *_)
{
    const ground_atom_t *ga = BOR_LIST_ENTRY(key, ground_atom_t, htable);
    char *d;
    size_t size;
    size = bor_offsetof(ground_atom_t, arg)
            - bor_offsetof(ground_atom_t, pred)
            + sizeof(pddl_cond_atom_arg_t) * ga->arg_size;
    d = (char *)ga;
    d += bor_offsetof(ground_atom_t, pred);
    return borCityHash_64(d, size);
}

static int groundAtomEq(const bor_list_t *k1, const bor_list_t *k2, void *_)
{
    const ground_atom_t *ga1 = BOR_LIST_ENTRY(k1, ground_atom_t, htable);
    const ground_atom_t *ga2 = BOR_LIST_ENTRY(k2, ground_atom_t, htable);
    return ga1->pred == ga2->pred
            && ga1->arg_size == ga2->arg_size
            && memcmp(ga1->arg, ga2->arg,
                      sizeof(pddl_cond_atom_arg_t) * ga1->arg_size) == 0;
}

static void groundAtomsInit(ground_atoms_t *gas)
{
    bzero(gas, sizeof(*gas));
    gas->alloc = 16;
    gas->atom = BOR_ALLOC_ARR(ground_atom_t *, gas->alloc);
    gas->htable = borHTableNew(groundAtomHash, groundAtomEq, NULL);
}

static void groundAtomsFree(ground_atoms_t *gas)
{
    borHTableDel(gas->htable);
    for (int i = 0; i < gas->size; ++i)
        groundAtomDel(gas->atom[i]);
    if (gas->atom)
        BOR_FREE(gas->atom);
}

static int groundAtomsAdd(ground_atoms_t *gas, ground_atom_t *ga)
{
    bor_list_t *found;
    ground_atom_t *found_ga;

    if ((found = borHTableFind(gas->htable, &ga->htable)) == NULL){
        if (gas->size >= gas->alloc){
            gas->alloc *= 2;
            gas->atom = BOR_REALLOC_ARR(gas->atom, ground_atom_t *,
                                        gas->alloc);
        }
        ga->id = gas->size;
        gas->atom[gas->size++] = ga;
        borHTableInsert(gas->htable, &ga->htable);
        return ga->id;

    }else{
        groundAtomDel(ga);
        found_ga = BOR_LIST_ENTRY(found, ground_atom_t, htable);
        return found_ga->id;
    }
}

static int groundAtomsAddFromAtom(ground_atoms_t *gas,
                                  const pddl_cond_atom_t *a)
{
    ground_atom_t *ga = groundAtomFromAtom(a);
    return groundAtomsAdd(gas, ga);
}

static int groundAtomsAddUnified(ground_atoms_t *gas,
                                 const ground_atom_t *from,
                                 const int *arg)
{
    ground_atom_t *ga = groundAtomUnify(from, arg);
    return groundAtomsAdd(gas, ga);
}

static bor_htable_key_t groundActionHash(const bor_list_t *key, void *_)
{
    const ground_action_t *a = BOR_LIST_ENTRY(key, ground_action_t, htable);
    char *d;
    size_t size;
    size = bor_offsetof(ground_action_t, data)
            - bor_offsetof(ground_action_t, action_id)
            + sizeof(int) * a->arg_size
            + sizeof(int) * a->ground_atom_size;
    d = (char *)a;
    d += bor_offsetof(ground_action_t, action_id);
    return borCityHash_64(d, size);
}

static int groundActionEq(const bor_list_t *k1, const bor_list_t *k2, void *_)
{
    const ground_action_t *ga1 = BOR_LIST_ENTRY(k1, ground_action_t, htable);
    const ground_action_t *ga2 = BOR_LIST_ENTRY(k2, ground_action_t, htable);
    return ga1->action_id == ga2->action_id
            && ga1->ground_atom_size == ga2->ground_atom_size
            && memcmp(ga1->data, ga2->data,
                      sizeof(int) * ga1->arg_size
                        + sizeof(int) * ga1->ground_atom_size) == 0;
}

static ground_action_t *groundActionFromAction(ground_t *g, int aid)
{
    const action_t *a = g->action.action + aid;
    ground_action_t *ga;
    size_t alloc;
    const pddl_cond_atom_t *atom;
    int *ground_atom, *arg;

    alloc  = bor_offsetof(ground_action_t, data);
    alloc += sizeof(int) * a->pre.size;
    alloc += sizeof(int) * a->param_size;
    ga = BOR_MALLOC(alloc);

    ga->action_id = aid;
    ga->arg_size = a->param_size;
    ga->ground_atom_size = a->pre.size;

    ground_atom = GROUND_ACTION_GROUND_ATOM(ga);
    for (int i = 0; i < a->pre.size; ++i){
        atom = PDDL_COND_CAST(a->pre.cond[i], atom);
        ground_atom[i] = groundAtomsAddFromAtom(&g->ground_atom, atom);
    }

    arg = GROUND_ACTION_ARG(ga);
    for (int i = 0; i < ga->arg_size; ++i)
        arg[i] = -1;
    return ga;
}

static ground_action_t *groundActionUnify(ground_t *g,
                                          const ground_action_t *from,
                                          const pddl_fact_t *fact,
                                          int atom_id)
{
    ground_atom_t *gatom;
    ground_action_t *ga;
    size_t alloc;
    int *ground_atom, *arg;
    const int *from_ground_atom, *from_arg;

    alloc  = bor_offsetof(ground_action_t, data);
    alloc += sizeof(int) * from->arg_size;
    alloc += sizeof(int) * (from->ground_atom_size - 1);
    ga = BOR_MALLOC(alloc);
    ga->id = -1;
    ga->action_id = from->action_id;
    ga->arg_size = from->arg_size;
    ga->ground_atom_size = from->ground_atom_size - 1;

    from_arg = GROUND_ACTION_ARG(from);
    from_ground_atom = GROUND_ACTION_GROUND_ATOM(from);
    arg = GROUND_ACTION_ARG(ga);
    ground_atom = GROUND_ACTION_GROUND_ATOM(ga);

    memcpy(arg, from_arg, sizeof(int) * ga->arg_size);
    gatom = g->ground_atom.atom[from_ground_atom[atom_id]];
    for (int i = 0; i < fact->arg_size; ++i){
        if (gatom->arg[i].param >= 0)
            arg[gatom->arg[i].param] = fact->arg[i];
    }

    for (int i = 0, j = 0; i < from->ground_atom_size; ++i){
        if (i == atom_id)
            continue;
        gatom = g->ground_atom.atom[from_ground_atom[i]];
        ground_atom[j++] = groundAtomsAddUnified(&g->ground_atom, gatom, arg);
    }

    /*
    fprintf(stdout, "UNIFY-FROM: ");
    groundActionPrint(g, from, stdout);
    fprintf(stdout, "UNIFY-TO[%d]: ", atom_id);
    groundActionPrint(g, ga, stdout);
    */

    return ga;
}

static void groundActionDel(ground_action_t *ga)
{
    BOR_FREE(ga);
}

static void groundActionPrint(ground_t *g, const ground_action_t *ga,
                              FILE *fout)
{
    const action_t *a = g->action.action + ga->action_id;
    const ground_atom_t *gatom;
    const int *ga_arg = GROUND_ACTION_ARG(ga);
    const int *ga_ground_atom = GROUND_ACTION_GROUND_ATOM(ga);

    fprintf(fout, "%d:(%s", ga->id, a->action->name);
    for (int i = 0; i < ga->arg_size; ++i){
        if (ga_arg[i] >= 0){
            fprintf(fout, " %s", g->pddl->obj.obj[ga_arg[i]].name);
        }else{
            fprintf(fout, " %s", a->action->param.param[i].name);
        }
    }
    fprintf(fout, ") ::");
    for (int i = 0; i < ga->ground_atom_size; ++i){
        gatom = g->ground_atom.atom[ga_ground_atom[i]];
        fprintf(fout, " [%d:%s:", i, g->pddl->pred.pred[gatom->pred].name);
        for (int j = 0; j < gatom->arg_size; ++j){
            if (gatom->arg[j].obj >= 0){
                fprintf(fout, " %s", g->pddl->obj.obj[gatom->arg[j].obj].name);
            }else{
                fprintf(fout, " %s",
                        a->action->param.param[gatom->arg[j].param].name);
            }
        }
        fprintf(fout, "]");
    }
    fprintf(fout, "\n");
}


static void groundActionsInit(ground_actions_t *gas)
{
    bzero(gas, sizeof(*gas));
    gas->alloc = 4;
    gas->action = BOR_ALLOC_ARR(ground_action_t *, gas->alloc);
    gas->htable = borHTableNew(groundActionHash, groundActionEq, NULL);
}

static void groundActionsFree(ground_actions_t *gas)
{
    borHTableDel(gas->htable);
    for (int i = 0; i < gas->size; ++i)
        groundActionDel(gas->action[i]);
    if (gas->action)
        BOR_FREE(gas->action);
}

static int groundActionsAdd(ground_actions_t *gas, ground_action_t *ga,
                            int *ga_id)
{
    bor_list_t *found;
    ground_action_t *found_ga;

    if ((found = borHTableFind(gas->htable, &ga->htable)) == NULL){
        if (gas->size >= gas->alloc){
            gas->alloc *= 2;
            gas->action = BOR_REALLOC_ARR(gas->action,
                                          ground_action_t *, gas->alloc);
        }
        ga->id = gas->size;
        gas->action[gas->size++] = ga;
        borHTableInsert(gas->htable, &ga->htable);
        if (ga_id)
            *ga_id = ga->id;
        return 0;

    }else{
        groundActionDel(ga);
        if (ga_id){
            found_ga = BOR_LIST_ENTRY(found, ground_action_t, htable);
            *ga_id = found_ga->id;
        }
        return -1;
    }
}

static int groundActionsAddFromAction(ground_t *g, ground_actions_t *gas,
                                      int aid, int *ga_id)
{
    ground_action_t *ga = groundActionFromAction(g, aid);
    return groundActionsAdd(gas, ga, ga_id);
}

static int groundActionsAddUnified(ground_t *g,
                                   ground_actions_t *gas,
                                   const ground_action_t *from,
                                   const pddl_fact_t *fact,
                                   int atom_id,
                                   int *ga_id)
{
    ground_action_t *ga;
    ga = groundActionUnify(g, from, fact, atom_id);
    return groundActionsAdd(gas, ga, ga_id);
}

static void predMapInit(pred_map_t *map, const pddl_t *pddl)
{
    bzero(map, sizeof(*map));
    map->pred_size = pddl->pred.size;
    map->map_ground_action = BOR_CALLOC_ARR(int *, map->pred_size);
    map->map_pre_id = BOR_CALLOC_ARR(int *, map->pred_size);
    map->map_size = BOR_CALLOC_ARR(int , map->pred_size);
    map->map_alloc = BOR_CALLOC_ARR(int , map->pred_size);
    for (int i = 0; i < map->pred_size; ++i){
        map->map_alloc[i] = 4;
        map->map_ground_action[i] = BOR_ALLOC_ARR(int, map->map_alloc[i]);
        map->map_pre_id[i] = BOR_ALLOC_ARR(int, map->map_alloc[i]);
    }
}

static void predMapFree(pred_map_t *map)
{
    for (int i = 0; i < map->pred_size; ++i){
        if (map->map_ground_action[i])
            BOR_FREE(map->map_ground_action[i]);
        if (map->map_pre_id[i])
            BOR_FREE(map->map_pre_id[i]);
    }
    if (map->map_ground_action)
        BOR_FREE(map->map_ground_action);
    if (map->map_pre_id)
        BOR_FREE(map->map_pre_id);
    if (map->map_size)
        BOR_FREE(map->map_size);
    if (map->map_alloc)
        BOR_FREE(map->map_alloc);
}

static void predMapAdd(pred_map_t *map, int pred_id, int ga_id, int pre_id)
{
    if (map->map_size[pred_id] >= map->map_alloc[pred_id]){
        map->map_alloc[pred_id] *= 2;
        map->map_ground_action[pred_id]
            = BOR_REALLOC_ARR(map->map_ground_action[pred_id],
                              int, map->map_alloc[pred_id]);
        map->map_pre_id[pred_id]
            = BOR_REALLOC_ARR(map->map_pre_id[pred_id],
                              int, map->map_alloc[pred_id]);
    }
    map->map_ground_action[pred_id][map->map_size[pred_id]] = ga_id;
    map->map_pre_id[pred_id][map->map_size[pred_id]] = pre_id;
    ++map->map_size[pred_id];
}

static void groundInitStaticFact(ground_t *g, const pddl_t *pddl)
{
    const pddl_fact_t *fact;

    for (int i = 0; i < pddl->init_fact.fact_size; ++i){
        fact = pddl->init_fact.fact[i];
        if (pddlFactIsStatic(pddl, fact)){
            pddlFactsAdd(&g->static_fact, fact);
            pddlFactsAdd(&g->fact, fact);
        }
    }

    if (pddl->pred.eq_pred >= 0){
        for (int i = 0; i < pddl->obj.size; ++i){
            PDDL_FACT_FOR_GROUND2(eq_fact, 2);
            eq_fact.pred = pddl->pred.eq_pred;
            eq_fact.arg_size = 2;
            eq_fact.arg[0] = i;
            eq_fact.arg[1] = i;
            pddlFactsAdd(&g->static_fact, &eq_fact);
            pddlFactsAdd(&g->fact, &eq_fact);
        }
    }
}

static void groundInitFact(ground_t *g, const pddl_t *pddl)
{
    const pddl_fact_t *fact;

    for (int i = 0; i < pddl->init_fact.fact_size; ++i){
        fact = pddl->init_fact.fact[i];
        if (!pddlFactIsStatic(pddl, fact)){
            pddlFactsAdd(&g->fact, fact);
        }
    }
}

static void groundAddGroundActionToTable(ground_t *g,
                                         const ground_action_t *ga)
{
    ground_atom_t *gatom;
    int pred_id;
    const int *ga_ground_atom = GROUND_ACTION_GROUND_ATOM(ga);

    for (int i = 0; i < ga->ground_atom_size; ++i){
        gatom = g->ground_atom.atom[ga_ground_atom[i]];
        pred_id = gatom->pred;
        predMapAdd(&g->pred_map, pred_id, ga->id, i);
        fprintf(stdout, "    --> table [pred: %d, id: %d, pre_id: %d]\n",
                pred_id, ga->id, i);
    }
}

static void _groundEnqueueAddEff(ground_t *g,
                                 ground_action_t *ga,
                                 int argi)
{
    const action_t *a = g->action.action + ga->action_id;
    int *ga_arg = GROUND_ACTION_ARG(ga);

    // Skip bound arguments
    for (; argi < ga->arg_size && ga_arg[argi] >= 0; ++argi);

    if (argi >= ga->arg_size){
        const pddl_cond_atom_t *atom;
        PDDL_FACT_FOR_GROUND2(fact, a->max_arg_size);

        for (int i = 0; i < a->add_eff.size; ++i){
            atom = PDDL_COND_CAST(a->add_eff.cond[i], atom);
            pddlCondAtomGroundFact(atom, ga_arg, &fact);
            pddlFactsAdd(&g->fact, &fact);
            fprintf(stdout, "  FACT ");
            pddlFactPrint(g->pddl, &fact, stdout);
            fprintf(stdout, "\n");
        }

    }else{
        const int *obj;
        int size;
        obj = pddlTypesObjsByType(&g->pddl->type,
                                  a->action->param.param[argi].type, &size);
        for (int i = 0; i < size; ++i){
            ga_arg[argi] = obj[i];
            _groundEnqueueAddEff(g, ga, argi + 1);
        }
        ga_arg[argi] = -1;
    }
}

static void groundEnqueueAddEff(ground_t *g, ground_action_t *ga)
{
    _groundEnqueueAddEff(g, ga, 0);
}

static void groundInitGroundActions(ground_t *g, const pddl_t *pddl)
{
    ground_action_t *ga;
    int gid;

    for (int i = 0; i < g->action.size; ++i){
        if (groundActionsAddFromAction(g, &g->ground_action, i, &gid) != 0)
            continue;
        ga = g->ground_action.action[gid];
        groundAddGroundActionToTable(g, ga);
        groundActionPrint(g, ga, stdout);

        if (ga->ground_atom_size == 0)
            groundEnqueueAddEff(g, ga);
    }
}

static void groundInit(ground_t *g, const pddl_t *pddl)
{
    bzero(g, sizeof(*g));
    g->pddl = pddl;
    actionsInit(&g->action, pddl);
    groundAtomsInit(&g->ground_atom);
    groundActionsInit(&g->ground_action);
    predMapInit(&g->pred_map, pddl);
    pddlFactsInit(&g->static_fact);
    pddlFactsInit(&g->fact);
    pddlStripsOpsInit(&g->op);

    groundInitStaticFact(g, pddl);
    groundInitFact(g, pddl);
    groundInitGroundActions(g, pddl);

    pddlFactsPrintInit(pddl, &g->static_fact, stdout);
    pddlFactsPrintInit(pddl, &g->fact, stdout);
}

static void groundFree(ground_t *g)
{
    actionsFree(&g->action);
    groundAtomsFree(&g->ground_atom);
    groundActionsFree(&g->ground_action);
    predMapFree(&g->pred_map);

    pddlFactsFree(&g->static_fact);
    pddlFactsFree(&g->fact);
    pddlStripsOpsFree(&g->op);
}

static int groundGroundActionCheckAtom(ground_t *g,
                                       const ground_action_t *ga,
                                       const pddl_fact_t *fact,
                                       const ground_atom_t *gatom)
{
    const action_t *a = g->action.action + ga->action_id;
    int param_id, param_type, obj_id;

    for (int i = 0; i < fact->arg_size; ++i){
        obj_id = fact->arg[i];
        if (gatom->arg[i].obj >= 0){
            if (obj_id != gatom->arg[i].obj)
                return -1;
        }else{
            param_id = gatom->arg[i].param;
            param_type = a->action->param.param[param_id].type;
            if (!pddlTypesObjHasType(&g->pddl->type, param_type, obj_id))
                return -1;
        }
    }

    return 0;
}

static int groundGroundActionCheckNegPre(ground_t *g,
                                         const ground_action_t *ga,
                                         const pddl_fact_t *fact,
                                         const ground_atom_t *gatom)
{
    const action_t *a = g->action.action + ga->action_id;
    if (a->pre_neg.size == 0)
        return 0;

    PDDL_FACT_FOR_GROUND2(fground, a->max_arg_size);
    const int *ga_arg = GROUND_ACTION_ARG(ga);
    int arg[a->param_size];
    const pddl_cond_atom_t *atom;

    memcpy(arg, ga_arg, sizeof(int) * a->param_size);
    for (int i = 0; i < gatom->arg_size; ++i){
        if (gatom->arg[i].param >= 0)
            arg[gatom->arg[i].param] = fact->arg[i];
    }

    for (int i = 0; i < a->pre_neg.size; ++i){
        atom = PDDL_COND_CAST(a->pre_neg.cond[i], atom);
        if (pddlCondAtomGroundFact(atom, arg, &fground) == 0){
            if (pddlFactsFind(&g->static_fact, &fground) >= 0){
                return -1;
            }
        }
    }

    return 0;
}

static int groundUnify(ground_t *g,
                       const pddl_fact_t *fact,
                       const ground_action_t *ga,
                       int pre_id)
{
    const ground_atom_t *gatom;
    int ret, next_ga_id;
    ground_action_t *next_ga;
    const int *ga_ground_atom = GROUND_ACTION_GROUND_ATOM(ga);

    fprintf(stdout, "  try(%d): ", pre_id);
    groundActionPrint(g, ga, stdout);

    gatom = g->ground_atom.atom[ga_ground_atom[pre_id]];
    if (groundGroundActionCheckAtom(g, ga, fact, gatom) != 0)
        return -1;
    if (groundGroundActionCheckNegPre(g, ga, fact, gatom) != 0)
        return -1;

    ret = groundActionsAddUnified(g, &g->ground_action, ga, fact, pre_id,
                                  &next_ga_id);
    if (ret == 0){
        next_ga = g->ground_action.action[next_ga_id];
        if (next_ga->ground_atom_size == 0){
            // TODO
            fprintf(stdout, "ADD: ");
            groundActionPrint(g, next_ga, stdout);
            groundEnqueueAddEff(g, next_ga);
        }else{
            fprintf(stdout, "GA: ");
            groundActionPrint(g, next_ga, stdout);
            groundAddGroundActionToTable(g, next_ga);
        }
        return next_ga_id;
    }
    return -1;
}

static void groundFact(ground_t *g, const pddl_fact_t *fact)
{
    int pred_id = fact->pred;
    const int *gas, *pre_ids;
    ground_action_t *ga;

    fprintf(stdout, "groundFact: ");
    pddlFactPrint(g->pddl, fact, stdout);
    fprintf(stdout, "\n");

    for (int i = 0; i < g->pred_map.map_size[pred_id]; ++i){
        gas = g->pred_map.map_ground_action[pred_id];
        pre_ids = g->pred_map.map_pre_id[pred_id];
        ga = g->ground_action.action[gas[i]];
        groundUnify(g, fact, ga, pre_ids[i]);
    }
}

static void ground(ground_t *g)
{
    const pddl_fact_t *fact;

    for (int i = 0; i < g->fact.fact_size; ++i){
        fact = g->fact.fact[i];
        groundFact(g, fact);
    }
}

void groundStrips(pddl_strips_t *strips, const pddl_t *pddl)
{
    ground_t g;
    groundInit(&g, pddl);
    ground(&g);
    groundFree(&g);
}
