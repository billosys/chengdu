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
#include "pddl/config.h"
#include "pddl/pddl.h"
#include "pddl/action.h"
#include "err.h"


#define PDDL_ACTIONS_ALLOC_INIT 4


static int parseAction(pddl_t *pddl, const pddl_lisp_node_t *root)
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

    a = pddlActionsAdd(&pddl->action);
    a->name = root->child[1].value;
    for (i = 2; i < root->child_size; i += 2){
        n = root->child + i + 1;
        if (root->child[i].kw == PDDL_KW_AGENT){
            if (!(pddl->require & PDDL_REQUIRE_MULTI_AGENT)){
                ERRN2(root->child + i, ":agent is allowed only with"
                                       " :multi-agent requirement.");
                return -1;
            }

            ret = pddlParamsParseAgent(&a->param, root, i, &pddl->type);
            if (ret < 0)
                return -1;
            i = ret - 2;

        }else if (root->child[i].kw == PDDL_KW_PARAMETERS){
            if (pddlParamsParse(&a->param, n, &pddl->type) != 0)
                return -1;

        }else if (root->child[i].kw == PDDL_KW_PRE){
            a->pre = pddlCondParse(n, pddl, &a->param, a->name);
            if (a->pre == NULL)
                return -1;
            if (pddlCondCheckPre(a->pre, pddl->require, 1) != 0)
                return -1;
            pddlCondSetPredRead(a->pre, &pddl->pred);

        }else if (root->child[i].kw == PDDL_KW_EFF){
            a->eff = pddlCondParse(n, pddl, &a->param, a->name);
            if (a->eff == NULL)
                return -1;
            if (pddlCondCheckEff(a->eff, pddl->require, 1) != 0)
                return -1;
            pddlCondSetPredReadWriteEff(a->eff, &pddl->pred);

        }else{
            ERRN(root->child + i, "Invalid definition of action `%s'."
                                  " Unexpected token: %s",
                                  a->name, root->child[i].value);
            return -1;
        }
    }

    // Empty precondition is allowed meaning the action can be applied in
    // any state
    if (a->pre == NULL)
        a->pre = pddlCondEmptyPre();

    // TODO: Check compatibility of types of parameters and types of
    //       arguments of all predicates.
    //       --> Restrict types instead of disallowing such an action?

    return 0;
}

int pddlActionsParse(pddl_t *pddl)
{
    const pddl_lisp_node_t *root = &pddl->domain_lisp->root;
    const pddl_lisp_node_t *n;
    int i;

    for (i = 0; i < root->child_size; ++i){
        n = root->child + i;
        if (pddlLispNodeHeadKw(n) == PDDL_KW_ACTION){
            if (parseAction(pddl, n) != 0)
                return -1;
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

void pddlActionCopy(pddl_action_t *dst, const pddl_action_t *src)
{
    pddlActionInit(dst);
    dst->name = src->name;
    pddlParamsCopy(&dst->param, &src->param);
    if (src->pre != NULL)
        dst->pre = pddlCondClone(src->pre);
    if (src->eff != NULL)
        dst->eff = pddlCondClone(src->eff);
}

void pddlActionNormalize(pddl_action_t *a, const pddl_types_t *t)
{
    a->pre = pddlCondNormalize(a->pre, t);
    a->eff = pddlCondNormalize(a->eff, t);

    if (a->pre->type == PDDL_COND_ATOM)
        a->pre = pddlCondAtomToAnd(a->pre);
    if (a->eff->type == PDDL_COND_ATOM)
        a->eff = pddlCondAtomToAnd(a->eff);
}

pddl_action_t *pddlActionsAdd(pddl_actions_t *as)
{
    pddl_action_t *a;

    if (as->size == as->alloc){
        if (as->alloc == 0)
            as->alloc = PDDL_ACTIONS_ALLOC_INIT;
        as->alloc *= 2;
        as->action = BOR_REALLOC_ARR(as->action, pddl_action_t, as->alloc);
    }

    a = as->action + as->size;
    ++as->size;
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

void pddlActionSplit(pddl_action_t *a, pddl_actions_t *as)
{
    pddl_action_t *newa;
    pddl_cond_part_t *pre;
    pddl_cond_t *first_cond, *cond;
    bor_list_t *item;
    int aidx;

    if (a->pre->type != PDDL_COND_OR)
        return;

    pre = bor_container_of(a->pre, pddl_cond_part_t, cls);
    if (borListEmpty(&pre->part))
        return;

    item = borListNext(&pre->part);
    borListDel(item);
    first_cond = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
    a->pre = NULL;
    aidx = a - as->action;
    while (!borListEmpty(&pre->part)){
        item = borListNext(&pre->part);
        borListDel(item);
        cond = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        newa = pddlActionsAdd(as);
        pddlActionCopy(newa, as->action + aidx);
        newa->pre = cond;
    }
    as->action[aidx].pre = first_cond;

    pddlCondDel(&pre->cls);
}

void pddlActionAssertPreConjuction(pddl_action_t *a)
{
    bor_list_t *item;
    pddl_cond_part_t *pre;
    pddl_cond_t *c;

    if (a->pre->type != PDDL_COND_AND){
        fprintf(stderr, "Fatal Error: Precondition of the action `%s' is"
                        " not a conjuction.\n", a->name);
        exit(-1);
    }

    pre = bor_container_of(a->pre, pddl_cond_part_t, cls);
    BOR_LIST_FOR_EACH(&pre->part, item){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        if (c->type != PDDL_COND_ATOM){
            fprintf(stderr, "Fatal Error: Precondition of the action `%s' is"
                            " not a flatten conjuction (conjuction contains"
                            " something else besides atoms).\n", a->name);
            exit(-1);
        }
    }
}

void pddlActionPrint(const pddl_t *pddl, const pddl_action_t *a, FILE *fout)
{
    fprintf(fout, "    %s: ", a->name);
    pddlParamsPrint(&a->param, fout);
    fprintf(fout, "\n");

    fprintf(fout, "        pre: ");
    pddlCondPrint(pddl, a->pre, &a->param, fout);
    fprintf(fout, "\n");

    fprintf(fout, "        eff: ");
    pddlCondPrint(pddl, a->eff, &a->param, fout);
    fprintf(fout, "\n");
}

void pddlActionsPrint(const pddl_t *pddl,
                      const pddl_actions_t *actions,
                      FILE *fout)
{
    int i;

    fprintf(fout, "Action[%d]:\n", actions->size);
    for (i = 0; i < actions->size; ++i)
        pddlActionPrint(pddl, actions->action + i, fout);
}
