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

#ifndef __PDDL_ACTION_H__
#define __PDDL_ACTION_H__

#include <pddl/lisp.h>
#include <pddl/obj.h>
#include <pddl/param.h>
#include <pddl/cond.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * Lifted action
 */
struct pddl_action {
    const char *name;
    pddl_params_t param;
    pddl_cond_t *pre;
    pddl_cond_t *eff;
};
typedef struct pddl_action pddl_action_t;

struct pddl_actions {
    pddl_action_t *action;
    int size;
    int alloc;
};
typedef struct pddl_actions pddl_actions_t;

/**
 * Initializes empty action
 */
void pddlActionInit(pddl_action_t *a);

/**
 * Frees allocated memory
 */
void pddlActionFree(pddl_action_t *a);

/**
 * Creates an exact copy of the given action.
 */
void pddlActionCopy(pddl_action_t *dst, const pddl_action_t *src);

/**
 * Simplify .pre and .eff (see pddlCondSimplify()).
 */
void pddlActionSimplify(pddl_action_t *a, const pddl_type_obj_t *to);

/**
 * Parses actions from domain PDDL.
 */
int pddlActionsParse(const pddl_lisp_t *domain,
                     const pddl_types_t *types,
                     const pddl_objs_t *objs,
                     const pddl_type_obj_t *type_obj,
                     pddl_preds_t *predicates,
                     const pddl_preds_t *functions,
                     unsigned require,
                     pddl_actions_t *actions);

/**
 * Free allocated memory.
 */
void pddlActionsFree(pddl_actions_t *actions);

/**
 * Adds an empty action to the list.
 */
pddl_action_t *pddlActionsAdd(pddl_actions_t *as);

/**
 * Call pddlActionSimplify() on each action.
 */
void pddlActionsSimplify(pddl_actions_t *a, const pddl_type_obj_t *to);

/**
 * Simplify all actions and split them by disjunctions in .pre.
 */
void pddlActionsSimplifyAndSplit(pddl_actions_t *a, const pddl_type_obj_t *to);

void pddlActionsPrint(const pddl_actions_t *actions,
                      const pddl_objs_t *objs,
                      const pddl_preds_t *predicates,
                      const pddl_preds_t *functions,
                      FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_ACTION_H__ */
