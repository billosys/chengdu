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

    a = pddlActionsAdd(actions);
    a->name = root->child[1].value;
    for (i = 2; i < root->child_size; i += 2){
        n = root->child + i + 1;
        if (root->child[i].kw == PDDL_KW_AGENT){
            if (!(require & PDDL_REQUIRE_MULTI_AGENT)){
                ERRN2(root->child + i, ":agent is allowed only with"
                                       " :multi-agent requirement.");
                return -1;
            }

            ret = pddlParamsParseAgent(&a->param, root, i, types);
            if (ret < 0)
                return -1;
            i = ret - 2;

        }else if (root->child[i].kw == PDDL_KW_PARAMETERS){
            if (pddlParamsParse(&a->param, n, types) != 0)
                return -1;

        }else if (root->child[i].kw == PDDL_KW_PRE){
            a->pre = pddlCondParse(n, types, objs, type_obj, predicates,
                                   functions, &a->param, a->name);
            if (a->pre == NULL)
                return -1;
            if (pddlCondCheckPre(a->pre, require, 1) != 0)
                return -1;
            if (pddlCondFlatten(a->pre) != 0)
                return -1;

        }else if (root->child[i].kw == PDDL_KW_EFF){
            a->eff = pddlCondParse(n, types, objs, type_obj, predicates,
                                   functions, &a->param, a->name);
            if (a->eff == NULL)
                return -1;
            if (pddlCondCheckEff(a->eff, require, 1) != 0)
                return -1;
            //if (pddlCondFlatten(a->eff) != 0)
            //    return -1;

        }else{
            ERRN(root->child + i, "Invalid definition of action `%s'."
                                  " Unexpected token: %s",
                                  a->name, root->child[i].value);
            return -1;
        }
    }

    // TODO: Check compatibility of types of parameters and types of
    //       arguments of all predicates.

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
                            functions, require, n, actions) != 0){
                return -1;
            }
        }
    }
    return 0;
}

void pddlActionInit(pddl_action_t *a)
{
    bzero(a, sizeof(*a));
    pddlParamsInit(&a->param);
}

void pddlActionFree(pddl_action_t *a)
{
    pddlParamsFree(&a->param);
    if (a->pre != NULL)
        pddlCondDel(a->pre);
    if (a->eff != NULL)
        pddlCondDel(a->eff);
}

pddl_action_t *pddlActionsAdd(pddl_actions_t *as)
{
    pddl_action_t *a;

    ++as->size;
    as->action = BOR_REALLOC_ARR(as->action, pddl_action_t, as->size);
    a = as->action + as->size - 1;
    pddlActionInit(a);
    return a;
}

void pddlActionsFree(pddl_actions_t *actions)
{
    pddl_action_t *a;
    int i;

    for (i = 0; i < actions->size; ++i){
        a = actions->action + i;
        pddlActionFree(a);
    }
    if (actions->action != NULL)
        BOR_FREE(actions->action);
}


static void pddlActionPrint(const pddl_action_t *a,
                            const pddl_objs_t *objs,
                            const pddl_preds_t *predicates,
                            const pddl_preds_t *functions,
                            FILE *fout)
{
    fprintf(fout, "    %s: ", a->name);
    pddlParamsPrint(&a->param, fout);
    fprintf(fout, "\n");

    fprintf(fout, "        pre: ");
    pddlCondPrint(a->pre, objs, predicates, functions, &a->param, fout);
    fprintf(fout, "\n");

    fprintf(fout, "        eff: ");
    pddlCondPrint(a->eff, objs, predicates, functions, &a->param, fout);
    fprintf(fout, "\n");
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
