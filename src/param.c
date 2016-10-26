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

#include "pddl/param.h"
#include "err.h"

void pddlParamCopy(pddl_param_t *dst, const pddl_param_t *src)
{
    *dst = *src;
}

void pddlParamsInit(pddl_params_t *params)
{
    bzero(params, sizeof(*params));
}

void pddlParamsFree(pddl_params_t *params)
{
    if (params->param != NULL)
        BOR_FREE(params->param);
}

pddl_param_t *pddlParamsAdd(pddl_params_t *params)
{
    pddl_param_t *param;

    if (params->size >= params->alloc){
        if (params->alloc == 0)
            params->alloc = 1;
        params->alloc *= 2;
        params->param = BOR_REALLOC_ARR(params->param, pddl_param_t,
                                        params->alloc);
    }

    param = params->param + params->size++;
    bzero(param, sizeof(*param));
    return param;
}

int pddlParamsGetId(const pddl_params_t *param, const char *name)
{
    int i;

    for (i = 0; i < param->size; ++i){
        if (strcmp(name, param->param[i].name) == 0)
            return i;
    }

    return -1;
}

struct _set_param_t {
    pddl_params_t *param;
    const pddl_types_t *types;
};
typedef struct _set_param_t set_param_t;

static int setParams(const pddl_lisp_node_t *root,
                     int child_from, int child_to, int child_type, void *ud)
{
    pddl_params_t *params = ((set_param_t *)ud)->param;
    const pddl_types_t *types = ((set_param_t *)ud)->types;
    pddl_param_t *param;
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

        param = pddlParamsAdd(params);
        param->name = root->child[i].value;
        param->type = tid;
        param->is_agent = 0;
    }

    return 0;
}

int pddlParamsParse(pddl_params_t *params,
                    const pddl_lisp_node_t *root,
                    const pddl_types_t *types)
{
    set_param_t set_param;
    set_param.param = params;
    set_param.types = types;
    if (pddlLispParseTypedList(root, 0, root->child_size,
                                setParams, &set_param) != 0)
        return -1;
    return 0;
}

int pddlParamsParseAgent(pddl_params_t *params,
                         const pddl_lisp_node_t *n,
                         int nid,
                         const pddl_types_t *types)
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

    set_param.param = params;
    set_param.types = types;
    if (pddlLispParseTypedList(n, nid + 1, to, setParams, &set_param) != 0)
        return -1;

    params->param[params->size - 1].is_agent = 1;
    return to;
}

void pddlParamsPrint(const pddl_params_t *params, FILE *fout)
{
    int i;

    for (i = 0; i < params->size; ++i){
        if (i > 0)
            fprintf(fout, " ");
        if (params->param[i].is_agent)
            fprintf(fout, "A:");
        if (params->param[i].inherit)
            fprintf(fout, "I:");
        fprintf(fout, "%s:%d", params->param[i].name, params->param[i].type);
    }
}
