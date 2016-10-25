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
#include "pddl/action.h"
#include "err.h"

struct _set_param_t {
    pddl_action_params_t *param;
    const pddl_types_t *types;
};
typedef struct _set_param_t set_param_t;


static void paramsFree(pddl_action_params_t *ps);
static pddl_action_param_t *paramsAdd(pddl_action_params_t *ps);


static void predFree(pddl_action_pred_t *p);
static void predCopy(pddl_action_pred_t *dst, const pddl_action_pred_t *src);

static void predsFree(pddl_action_preds_t *ps);
static pddl_action_pred_t *predsAdd(pddl_action_preds_t *ps);
/** Free unneeded memory */
static void predsSqueeze(pddl_action_preds_t *ps);
static void predsCopy(pddl_action_preds_t *dst, const pddl_action_preds_t *src);
static int predNonNegCmp(const pddl_action_pred_t *p1,
                         const pddl_action_pred_t *p2);
static int predCmp(const pddl_action_pred_t *p1, const pddl_action_pred_t *p2);
static void sortPreds(pddl_action_preds_t *eff);
/** Keep only predicates having .neg != 2 */
static void reorderPreds(pddl_action_preds_t *ps);
static int predInPreds(const pddl_action_pred_t *p,
                       const pddl_action_preds_t *ps);


void condEffFree(pddl_action_cond_eff_t *ce);
void condEffsFree(pddl_action_cond_effs_t *ce);
void condEffCopy(pddl_action_cond_eff_t *dst,
                 const pddl_action_cond_eff_t *src);
void condEffsCopy(pddl_action_cond_effs_t *dst,
                  const pddl_action_cond_effs_t *src);


static int parsePreEff(const pddl_types_t *types,
                       const pddl_objs_t *objs,
                       const pddl_type_obj_t *type_obj,
                       const pddl_preds_t *predicates,
                       const pddl_preds_t *functions,
                       unsigned require,
                       const pddl_lisp_node_t *root,
                       const char *errname,
                       pddl_action_t *a,
                       pddl_action_preds_t *preds,
                       int parse_cost, int parse_cond_eff);

static pddl_action_t *addAction(pddl_actions_t *as, const char *name)
{
    pddl_action_t *a;
    a = pddlActionsAdd(as);
    a->name = name;
    return a;
}

static pddl_action_cond_eff_t *addCondEff(pddl_action_t *a)
{
    pddl_action_cond_eff_t *ce;

    ++a->cond_eff.size;
    a->cond_eff.cond_eff = BOR_REALLOC_ARR(a->cond_eff.cond_eff,
                                           pddl_action_cond_eff_t,
                                           a->cond_eff.size);
    ce = a->cond_eff.cond_eff + a->cond_eff.size - 1;
    bzero(ce, sizeof(*ce));
    return ce;
}

static int getParam(const pddl_action_t *a, const char *var_name)
{
    int i;

    for (i = 0; i < a->param.size; ++i){
        if (strcmp(a->param.param[i].name, var_name) == 0)
            return i;
    }
    return -1;
}

static int setParams(const pddl_lisp_node_t *root,
                     int child_from, int child_to, int child_type, void *ud)
{
    pddl_action_params_t *params = ((set_param_t *)ud)->param;
    const pddl_types_t *types = ((set_param_t *)ud)->types;
    pddl_action_param_t *param;
    int i, tid;

    tid = 0;
    if (child_type >= 0){
        tid = pddlTypesGet(types, root->child[child_type].value);
        if (tid < 0){
            ERRN(root->child + child_type, "Unkown type `%s'",
                 root->child[child_type].value);
            return -1;
        }
    }

    for (i = child_from; i < child_to; ++i){
        if (root->child[i].value == NULL){
            ERRN2(root->child + i, "Invalid parameter definition:"
                                   " Unexpected expression.");
            return -1;
        }

        if (root->child[i].value[0] != '?'){
            ERRN(root->child + i, "Invalid parameter definition:"
                                  " Expected variable, got %s.",
                 root->child[i].value);
            return -1;
        }

        param = paramsAdd(params);
        param->name = root->child[i].value;
        param->type = tid;
        param->is_agent = 0;
    }

    return 0;
}


static void simplifyAction(pddl_action_t *a)
{
    pddl_action_pred_t *p1, *p2;
    int i, cmp, del;

    // First sort facts in precondition and effect
    sortPreds(&a->pre);
    sortPreds(&a->eff);

    // Delete duplicate preconditions
    del = 0;
    for (i = 1; i < a->pre.size; ++i){
        p1 = a->pre.pred + i - 1;
        p2 = a->pre.pred + i;

        cmp = predNonNegCmp(p1, p2);
        if (cmp == 0 && p1->neg == p2->neg){
            // Remove duplicate predicates
            predFree(p1);
            p1->neg = 2;
            del = 1;
        }
    }

    if (del)
        reorderPreds(&a->pre);

    del = 0;
    for (i = 1; i < a->eff.size; ++i){
        p1 = a->eff.pred + i - 1;
        p2 = a->eff.pred + i;

        cmp = predNonNegCmp(p1, p2);
        if (cmp == 0 && p1->neg != p2->neg){
            // Remove delete effects if they are also in add effects.
            // This is "add-after-delete semantics" which seems to be
            // accepted as de-facto standard -- see rovers domain.

            // Delete effect
            predFree(p1);
            // and use .neg flag as "remove it later flag"
            p1->neg = 2;
            // Force the second fact to positive fact because we use
            // "add-after-delete" semantics.
            p2->neg = 0;
            del = 1;

        }else if (cmp == 0 && p1->neg == p2->neg){
            // Remove duplicate facts
            predFree(p1);
            p1->neg = 2;
            del = 1;
        }
    }

    // Delete effects that are already in preconditions
    for (i = 0; i < a->eff.size; ++i){
        p1 = a->eff.pred + i;
        if (p1->neg == 2)
            continue;

        if (predInPreds(p1, &a->pre)){
            predFree(p1);
            p1->neg = 2;
            del = 1;
        }
    }

    if (del)
        reorderPreds(&a->eff);
}

static int parseAgentParams(const pddl_types_t *types,
                            const pddl_lisp_node_t *n, int nid,
                            pddl_action_params_t *param)
{
    set_param_t set_param;
    int to;

    if (nid + 2 < n->child_size
            && n->child[nid + 2].value != NULL
            && n->child[nid + 2].value[0] == '-'){
        to = nid + 4;
    }else{
        to = nid + 2;
    }

    set_param.param = param;
    set_param.types = types;
    if (pddlLispParseTypedList(n, nid + 1, to, setParams, &set_param) != 0)
        return -1;

    param->param[param->size - 1].is_agent = 1;
    return to;
}

static int parseParams(const pddl_types_t *types,
                       const pddl_lisp_node_t *n,
                       pddl_action_params_t *param)
{
    set_param_t set_param;
    set_param.param = param;
    set_param.types = types;
    if (pddlLispParseTypedList(n, 0, n->child_size,
                                   setParams, &set_param) != 0)
        return -1;
    return 0;
}

static int parseVarConst(const pddl_lisp_node_t *root,
                         const pddl_objs_t *objs,
                         const pddl_action_t *a,
                         pddl_action_pred_arg_t *dst)
{
    int v;

    if (root->value[0] == '?'){
        v = getParam(a, root->value);
        if (v < 0){
            ERRN(root, "Invalid paramenter name `%s'", root->value);
            return -1;
        }
        dst->param = v;
        dst->obj = -1;

    }else{
        v = pddlObjsGet(objs, root->value);
        if (v < 0){
            ERRN(root, "Unkown constant `%s'", root->value);
            return -1;
        }
        dst->param = -1;
        dst->obj = v;
    }

    return 0;
}

static pddl_action_pred_t *parsePred(const pddl_objs_t *objs,
                                     const pddl_preds_t *predicates,
                                     const pddl_lisp_node_t *root,
                                     pddl_action_t *a,
                                     pddl_action_preds_t *ps)
{
    pddl_action_pred_t *p;
    const char *name;
    int pred, i;

    // Get predicate name
    name = pddlLispNodeHead(root);
    if (name == NULL){
        ERRN2(root, "Invalid definition of conditional, missing head of"
                    " expression.");
        return NULL;
    }

    // And resolve it against known predicates
    pred = pddlPredsGet(predicates, name);
    if (pred == -1){
        ERRN(root, "Unkown predicate `%s'", name);
        return NULL;
    }

    // Check that all children are terminals
    for (i = 1; i < root->child_size; ++i){
        if (root->child[i].value == NULL){
            ERRN(root, "Invalid instantiation of predicate `%s'", name);
            return NULL;
        }
    }

    // Add fact to preconditions
    p = predsAdd(ps);
    p->pred = pred;
    p->arg_size = root->child_size - 1;
    p->arg = BOR_ALLOC_ARR(pddl_action_pred_arg_t, p->arg_size);
    for (i = 0; i < p->arg_size; ++i){
        if (parseVarConst(root->child + i + 1, objs, a, p->arg + i) != 0)
            return NULL;
    }

    return p;
}

static int parseCost(const pddl_objs_t *objs,
                     const pddl_preds_t *functions,
                     unsigned require,
                     const pddl_lisp_node_t *root,
                     pddl_action_t *a)
{
    pddl_action_pred_t *p;

    if (!(require & PDDL_REQUIRE_ACTION_COST)){
        ERRN2(root, "(increase ...) is supported only under"
                " :action-costs requirement.");
        return -1;
    }

    if (root->child_size != 3){
        ERRN2(root, "Invalid (increase ...) specification.");
        return -1;
    }

    if (root->child[1].value != NULL
            || root->child[1].child_size != 1
            || root->child[1].child[0].value == NULL
            || strcmp(root->child[1].child[0].value, "total-cost") != 0){
        ERRN2(root, "Invalid (increase ...) specification -- the first"
                    " argument must be (total-cost).");
        return -1;
    }

    if (root->child[2].value != NULL){
        p = predsAdd(&a->cost);
        p->pred = -1;
        p->func_val = atoi(root->child[2].value);
    }else{
        p = parsePred(objs, functions, root->child + 2, a, &a->cost);
        if (p == NULL)
            return -1;
    }

    return 0;
}

static int parseCondEff(const pddl_types_t *types,
                        const pddl_objs_t *objs,
                        const pddl_type_obj_t *type_obj,
                        const pddl_preds_t *predicates,
                        const pddl_lisp_node_t *root,
                        pddl_action_t *a)
{
    pddl_action_cond_eff_t *ce;

    if (root->child_size != 3
            || root->child[1].value != NULL
            || root->child[2].value != NULL){
        ERRN2(root, "Invalid (when ...) condition.");
        return -1;
    }

    ce = addCondEff(a);
    if (parsePreEff(types, objs, type_obj, predicates, NULL, 0, root->child + 1,
                    "cond-eff predicate", a, &ce->pre, 0, 0) != 0)
        return -1;
    if (parsePreEff(types, objs, type_obj, predicates, NULL, 0, root->child + 2,
                    "cond-eff effect", a, &ce->eff, 0, 0) != 0)
        return -1;
    return 0;
}

static void parseForallReplace(const pddl_action_params_t *params,
                               const int *bound_var,
                               const pddl_objs_t *objs,
                               pddl_lisp_node_t *n)
{
    int i;

    if (n->kw == -1 && n->value != NULL){
        for (i = 0; i < params->size; ++i){
            if (strcmp(n->value, params->param[i].name) == 0){
                n->value = objs->obj[bound_var[i]].name;
            }
        }
    }

    for (i = 0; i < n->child_size; ++i)
        parseForallReplace(params, bound_var, objs, n->child + i);
}

static int parseForallEval(const pddl_action_params_t *params,
                           int *bound_var,
                           const pddl_types_t *types,
                           const pddl_objs_t *objs,
                           const pddl_type_obj_t *type_obj,
                           const pddl_preds_t *predicates,
                           const pddl_preds_t *functions,
                           unsigned require,
                           const pddl_lisp_node_t *root,
                           const char *errname,
                           pddl_action_t *a,
                           pddl_action_preds_t *preds,
                           int parse_cost, int parse_cond_eff)
{
    pddl_lisp_node_t arg;
    int ret = 0;

    pddlLispNodeCopy(&arg, root->child + 2);
    parseForallReplace(params, bound_var, objs, &arg);
    ret = parsePreEff(types, objs, type_obj, predicates, functions,
                      require, &arg, errname, a, preds, parse_cost,
                      parse_cond_eff);
    pddlLispNodeFree(&arg);
    return ret;
}

static int parseForallRec(const pddl_action_params_t *params,
                          int *bound_var, int var_id,
                          const pddl_types_t *types,
                          const pddl_objs_t *objs,
                          const pddl_type_obj_t *type_obj,
                          const pddl_preds_t *predicates,
                          const pddl_preds_t *functions,
                          unsigned require,
                          const pddl_lisp_node_t *root,
                          const char *errname,
                          pddl_action_t *a,
                          pddl_action_preds_t *preds,
                          int parse_cost, int parse_cond_eff)
{
    const int *obj;
    int obj_size, i;

    obj = pddlTypeObjGet(type_obj, params->param[var_id].type, &obj_size);
    for (i = 0; i < obj_size; ++i){
        bound_var[var_id] = obj[i];
        if (var_id == params->size - 1){
            if (parseForallEval(params, bound_var, types, objs, type_obj,
                                predicates, functions, require, root,
                                errname, a, preds, parse_cost,
                                parse_cond_eff) != 0)
                return -1;
        }else{
            if (parseForallRec(params, bound_var, var_id + 1, types,
                               objs, type_obj, predicates, functions,
                               require, root, errname, a, preds,
                               parse_cost, parse_cond_eff) != 0)
                return -1;
        }
    }

    return 0;
}

static int parseForall(const pddl_types_t *types,
                       const pddl_objs_t *objs,
                       const pddl_type_obj_t *type_obj,
                       const pddl_preds_t *predicates,
                       const pddl_preds_t *functions,
                       unsigned require,
                       const pddl_lisp_node_t *root,
                       const char *errname,
                       pddl_action_t *a,
                       pddl_action_preds_t *preds,
                       int parse_cost, int parse_cond_eff)
{
    pddl_action_params_t params;
    int *bound_var;
    int ret;

    if (root->child_size != 3
            || root->child[1].value != NULL
            || root->child[2].value != NULL){
        ERRN2(root, "Invalid (forall ...) condition.");
        return -1;
    }

    bzero(&params, sizeof(params));
    if (parseParams(types, root->child + 1, &params) != 0){
        paramsFree(&params);
        return -1;
    }
    if (params.size == 0){
        ERRN2(root, "Missing arguments in (forall ...).");
        return -1;
    }

    bound_var = BOR_ALLOC_ARR(int, params.size);
    ret = parseForallRec(&params, bound_var, 0, types, objs, type_obj,
                         predicates, functions, require, root, errname, a, preds,
                         parse_cost, parse_cond_eff);

    BOR_FREE(bound_var);
    paramsFree(&params);
    return ret;
}

static int parsePreEff(const pddl_types_t *types,
                       const pddl_objs_t *objs,
                       const pddl_type_obj_t *type_obj,
                       const pddl_preds_t *predicates,
                       const pddl_preds_t *functions,
                       unsigned require,
                       const pddl_lisp_node_t *root,
                       const char *errname,
                       pddl_action_t *a,
                       pddl_action_preds_t *preds,
                       int parse_cost, int parse_cond_eff)
{
    pddl_action_pred_t *p;
    int i, kw;

    kw = pddlLispNodeHeadKw(root);
    if (kw == PDDL_KW_AND){
        for (i = 1; i < root->child_size; ++i){
            if (parsePreEff(types, objs, type_obj, predicates, functions,
                            require, root->child + i, errname, a, preds,
                            parse_cost, parse_cond_eff) != 0)
                return -1;
        }

    }else if (kw == PDDL_KW_NOT){
        if (root->child_size != 2
                || root->child[1].kw != -1){
            ERRN(root, "Only simple (not (predicate ...)) construct is"
                       " allowed in action %s.", errname);
            return -1;
        }

        if ((p = parsePred(objs, predicates, root->child + 1, a, preds)) == NULL)
            return -1;
        p->neg = 1;

    }else if (kw == PDDL_KW_FORALL){
        if (parseForall(types, objs, type_obj, predicates, functions,
                        require, root, errname, a, preds, parse_cost,
                        parse_cond_eff) != 0)
            return -1;

    }else if (kw == PDDL_KW_WHEN){
        if (!parse_cond_eff){
            ERRN(root, "(when ...) is not supported in %s.", errname);
            return 0;
        }

        if (parseCondEff(types, objs, type_obj, predicates, root, a) != 0)
            return -1;

    }else if (kw == PDDL_KW_OR){
        ERRN(root, "(or ...) is not supported in %s yet.", errname);
        return -1;

    }else if (kw == PDDL_KW_IMPLY){
        ERRN(root, "(imply ...) is not supported in %s yet.", errname);
        return -1;

    }else if (kw == PDDL_KW_EXISTS){
        ERRN(root, "(exists ...) is not supported in %s yet.", errname);
        return -1;

    }else if (kw == PDDL_KW_INCREASE){
        if (!parse_cost){
            ERRN(root, "(increase ...) is not supported in %s.", errname);
            return -1;
        }

        if (parseCost(objs, functions, require, root, a) != 0)
            return -1;

    }else if (kw == -1){
        if (parsePred(objs, predicates, root, a, preds) == NULL)
            return -1;

    }else{
        if (root->child_size >= 1
                && root->child[0].value != NULL){
            ERRN(root, "Unexpected token while parsing action"
                       " %s: `%s'", errname, root->child[0].value);
        }else{
            ERRN(root, "Unexpected token while parsing action %s.", errname);
        }
        return -1;
    }
    return 0;
}

static int parseAction(const pddl_types_t *types,
                       const pddl_objs_t *objs,
                       const pddl_type_obj_t *type_obj,
                       const pddl_preds_t *predicates,
                       const pddl_preds_t *functions,
                       unsigned require,
                       const pddl_lisp_node_t *root,
                       pddl_actions_t *actions)
{
    const pddl_lisp_node_t *n;
    pddl_action_t *a;
    int i, ret;

    if (root->child_size < 4
            || root->child_size / 2 == 1
            || root->child[1].value == NULL){
        ERRN2(root, "Invalid definition of :action");
        return -1;
    }

    a = addAction(actions, root->child[1].value);
    for (i = 2; i < root->child_size; i += 2){
        n = root->child + i + 1;
        if (root->child[i].kw == PDDL_KW_AGENT){
            if (!(require & PDDL_REQUIRE_MULTI_AGENT)){
                ERRN2(root->child + i, ":agent is allowed only with"
                                       " :multi-agent requirement.");
                return -1;
            }

            ret = parseAgentParams(types, root, i, &a->param);
            if (ret < 0)
                return -1;
            i = ret - 2;

        }else if (root->child[i].kw == PDDL_KW_PARAMETERS){
            if (parseParams(types, n, &a->param) != 0)
                return -1;

        }else if (root->child[i].kw == PDDL_KW_PRE){
            if (parsePreEff(types, objs, type_obj, predicates, functions,
                            require, n, "precondition", a, &a->pre, 0, 0) != 0)
                return -1;

        }else if (root->child[i].kw == PDDL_KW_EFF){
            if (parsePreEff(types, objs, type_obj, predicates, functions,
                            require, n, "effect", a, &a->eff, 1, 1) != 0)
                return -1;

        }else{
            ERRN(root->child + i, "Invalid definition of :action."
                                  " Unexpected token: %s",
                                  root->child[i].value);
            return -1;
        }
    }

    // TODO: Check compatibility of types of parameters and types of
    //       arguments of all predicates.
    simplifyAction(a);

    return 0;
}

int pddlActionsParse(const pddl_lisp_t *domain,
                     const pddl_types_t *types,
                     const pddl_objs_t *objs,
                     const pddl_type_obj_t *type_obj,
                     const pddl_preds_t *predicates,
                     const pddl_preds_t *functions,
                     unsigned require,
                     pddl_actions_t *actions)
{
    const pddl_lisp_node_t *root = &domain->root;
    const pddl_lisp_node_t *n;
    int i;

    for (i = 0; i < root->child_size; ++i){
        n = root->child + i;
        if (pddlLispNodeHeadKw(n) == PDDL_KW_ACTION){
            if (parseAction(types, objs, type_obj, predicates,
                            functions, require, n, actions) != 0)
                return -1;
        }
    }
    return 0;
}

pddl_action_t *pddlActionsAdd(pddl_actions_t *as)
{
    pddl_action_t *a;

    ++as->size;
    as->action = BOR_REALLOC_ARR(as->action, pddl_action_t, as->size);
    a = as->action + as->size - 1;
    bzero(a, sizeof(*a));
    return a;
}

void pddlActionsFree(pddl_actions_t *actions)
{
    int i;

    for (i = 0; i < actions->size; ++i){
        paramsFree(&actions->action[i].param);
        predsFree(&actions->action[i].pre);
        predsFree(&actions->action[i].eff);
        predsFree(&actions->action[i].cost);
        condEffsFree(&actions->action[i].cond_eff);
    }
    if (actions->action != NULL)
        BOR_FREE(actions->action);
}

void pddlActionPredPrint(const pddl_preds_t *predicates,
                         const pddl_objs_t *objs,
                         const pddl_action_t *a,
                         const pddl_action_pred_t *p,
                         FILE *fout)
{
    int j;

    if (p->neg)
        fprintf(fout, "N:");
    fprintf(fout, "%s:", predicates->pred[p->pred].name);
    for (j = 0; j < p->arg_size; ++j){
        if (p->arg[j].param < 0){
            fprintf(fout, " %s", objs->obj[p->arg[j].obj].name);
        }else{
            fprintf(fout, " %s", a->param.param[p->arg[j].param].name);
        }
    }
}

static void printPreds(const pddl_action_t *a,
                       const pddl_objs_t *objs,
                       const pddl_preds_t *predicates,
                       const pddl_action_preds_t *ps,
                       FILE *fout)
{
    const pddl_action_pred_t *p;
    int i;

    for (i = 0; i < ps->size; ++i){
        p = ps->pred + i;
        fprintf(fout, "            ");
        pddlActionPredPrint(predicates, objs, a, p, fout);
        fprintf(fout, "\n");
    }
}

static void pddlActionPrint(const pddl_action_t *a,
                            const pddl_objs_t *objs,
                            const pddl_preds_t *predicates,
                            const pddl_preds_t *functions,
                            FILE *fout)
{
    int i, j, id;

    fprintf(fout, "    %s:", a->name);
    for (i = 0; i < a->param.size; ++i){
        fprintf(fout, " ");
        if (a->param.param[i].is_agent)
            fprintf(fout, "A:");
        fprintf(fout, "%s:%d", a->param.param[i].name, a->param.param[i].type);
    }
    fprintf(fout, "\n");

    fprintf(fout, "        pre[%d]:\n", a->pre.size);
    printPreds(a, objs, predicates, &a->pre, fout);

    fprintf(fout, "        eff[%d]:\n", a->eff.size);
    printPreds(a, objs, predicates, &a->eff, fout);

    fprintf(fout, "        cond-eff[%d]:\n", a->cond_eff.size);
    for (i = 0; i < a->cond_eff.size; ++i){
        fprintf(fout, "          pre[%d]:\n",
                a->cond_eff.cond_eff[i].pre.size);
        printPreds(a, objs, predicates, &a->cond_eff.cond_eff[i].pre, fout);
        fprintf(fout, "          eff[%d]:\n",
                a->cond_eff.cond_eff[i].eff.size);
        printPreds(a, objs, predicates, &a->cond_eff.cond_eff[i].eff, fout);
    }

    for (i = 0; i < a->cost.size; ++i){
        fprintf(fout, "        cost: ");
        if (a->cost.pred[i].pred == -1){
            fprintf(fout, "%d", a->cost.pred[i].func_val);
        }else{
            fprintf(fout, "%s", functions->pred[a->cost.pred[i].pred].name);
            for (j = 0; j < a->cost.pred[i].arg_size; ++j){
                if (a->cost.pred[i].arg[j].param >= 0){
                    id = a->cost.pred[i].arg[j].param;
                    fprintf(fout, " %s", a->param.param[id].name);
                }else{
                    id = a->cost.pred[i].arg[j].obj;
                    fprintf(fout, " %s", objs->obj[id].name);
                }
            }
        }
        fprintf(fout, "\n");
    }
}

void pddlActionsPrint(const pddl_actions_t *actions,
                      const pddl_objs_t *objs,
                      const pddl_preds_t *predicates,
                      const pddl_preds_t *functions,
                      FILE *fout)
{
    int i;

    fprintf(fout, "Action[%d]:\n", actions->size);
    for (i = 0; i < actions->size; ++i)
        pddlActionPrint(actions->action + i, objs,
                            predicates, functions, fout);
}



/*** PARAM ***/
static void paramsFree(pddl_action_params_t *ps)
{
    if (ps->param != NULL)
        BOR_FREE(ps->param);
}

static pddl_action_param_t *paramsAdd(pddl_action_params_t *ps)
{
    pddl_action_param_t *p;

    if (ps->size >= ps->alloc){
        if (ps->alloc == 0){
            ps->alloc = 2;
        }else{
            ps->alloc *= 2;
        }
        ps->param = BOR_REALLOC_ARR(ps->param, pddl_action_param_t, ps->alloc);
    }

    p = ps->param + ps->size++;
    bzero(p, sizeof(*p));
    return p;
}


/*** PRED ***/
static void predFree(pddl_action_pred_t *p)
{
    if (p->arg != NULL)
        BOR_FREE(p->arg);
}

static void predCopy(pddl_action_pred_t *dst, const pddl_action_pred_t *src)
{
    *dst = *src;
    dst->arg = BOR_ALLOC_ARR(pddl_action_pred_arg_t, dst->arg_size);
    memcpy(dst->arg, src->arg, sizeof(pddl_action_pred_arg_t) * dst->arg_size);
}

static void predsFree(pddl_action_preds_t *ps)
{
    int i;

    for (i = 0; i < ps->size; ++i)
        predFree(ps->pred + i);
    if (ps->pred != NULL)
        BOR_FREE(ps->pred);
}

static pddl_action_pred_t *predsAdd(pddl_action_preds_t *ps)
{
    pddl_action_pred_t *p;

    if (ps->size >= ps->alloc){
        if (ps->alloc == 0){
            ps->alloc = 2;
        }else{
            ps->alloc *= 2;
        }
        ps->pred = BOR_REALLOC_ARR(ps->pred, pddl_action_pred_t, ps->alloc);
    }

    p = ps->pred + ps->size++;
    bzero(p, sizeof(*p));
    return p;
}

static void predsSqueeze(pddl_action_preds_t *ps)
{
    ps->alloc = ps->size;
    ps->pred = BOR_REALLOC_ARR(ps->pred, pddl_action_pred_t, ps->alloc);
}


static void predsCopy(pddl_action_preds_t *dst, const pddl_action_preds_t *src)
{
    int i;

    *dst = *src;
    if (src->pred != NULL)
        dst->pred = BOR_ALLOC_ARR(pddl_action_pred_t, src->size);
    for (i = 0; i < dst->size; ++i)
        predCopy(dst->pred + i, src->pred + i);
}

static int predArgCmp(const pddl_action_pred_arg_t *arg1,
                      const pddl_action_pred_arg_t *arg2,
                      int size)
{
    int i;

    for (i = 0; i < size; ++i){
        if (arg1[i].obj >= 0 && arg2[i].obj < 0)
            return 1;
        if (arg2[i].obj >= 0 && arg1[i].obj < 0)
            return -1;
        if (arg1[i].obj >= 0 && arg1[i].obj != arg2[i].obj)
            return arg1[i].obj - arg2[i].obj;
        if (arg1[i].param >= 0 && arg1[i].param != arg2[i].param)
            return arg1[i].param - arg2[i].param;
    }

    return 0;
}

static int predNonNegCmp(const pddl_action_pred_t *p1,
                         const pddl_action_pred_t *p2)
{
    int cmp;

    cmp = p1->pred - p2->pred;
    if (cmp == 0)
        cmp = p1->arg_size - p2->arg_size;
    if (cmp == 0)
        cmp = predArgCmp(p1->arg, p2->arg, p1->arg_size);
    return cmp;
}

static int predCmp(const pddl_action_pred_t *p1, const pddl_action_pred_t *p2)
{
    int cmp;

    cmp = predNonNegCmp(p1, p2);
    if (cmp == 0)
        cmp = p1->neg - p2->neg;
    return cmp;
}

static int sortPredsCmp(const void *a, const void *b)
{
    const pddl_action_pred_t *p1 = a;
    const pddl_action_pred_t *p2 = b;
    return predCmp(p1, p2);
}

static void sortPreds(pddl_action_preds_t *eff)
{
    // Sort effects so the same ones are next to each other
    qsort(eff->pred, eff->size, sizeof(pddl_action_pred_t), sortPredsCmp);
}

static void reorderPreds(pddl_action_preds_t *ps)
{
    int ins, i;

    for (ins = 0, i = 0; i < ps->size; ++i){
        if (ps->pred[i].neg != 2){
            ps->pred[ins++] = ps->pred[i];
        }
    }

    ps->size = ins;
    predsSqueeze(ps);
}

static int predInPreds(const pddl_action_pred_t *p,
                       const pddl_action_preds_t *ps)
{
    int i;

    for (i = 0; i < ps->size && ps->pred[i].pred <= p->pred; ++i){
        if (predCmp(p, ps->pred + i) == 0)
            return 1;
    }
    return 0;
}


/*** COND-EFF ***/
void condEffFree(pddl_action_cond_eff_t *ce)
{
    predsFree(&ce->pre);
    predsFree(&ce->eff);
}

void condEffsFree(pddl_action_cond_effs_t *ce)
{
    int i;

    for (i = 0; i < ce->size; ++i)
        condEffFree(ce->cond_eff + i);
    if (ce->cond_eff != NULL)
        BOR_FREE(ce->cond_eff);
}

void condEffCopy(pddl_action_cond_eff_t *dst,
                 const pddl_action_cond_eff_t *src)
{
    predsCopy(&dst->pre, &src->pre);
    predsCopy(&dst->eff, &src->eff);
}

void condEffsCopy(pddl_action_cond_effs_t *dst,
                  const pddl_action_cond_effs_t *src)
{
    int i;

    *dst = *src;
    if (src->cond_eff != NULL)
        dst->cond_eff = BOR_ALLOC_ARR(pddl_action_cond_eff_t, dst->size);
    for (i = 0; i < dst->size; ++i)
        condEffCopy(dst->cond_eff + i, src->cond_eff + i);
}
