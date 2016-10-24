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
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Action parameter
 */
struct pddl_action_param {
    const char *name;
    int type;
    int is_agent;
};
typedef struct pddl_action_param pddl_action_param_t;

struct pddl_action_params {
    pddl_action_param_t *param;
    int size;
    int alloc;
};
typedef struct pddl_action_params pddl_action_params_t;


/**
 * Predicate with arguments bound to an action parameter or a constant.
 */
struct pddl_action_pred {
    int pred; /*!< Predicate ID */
    int *arg; /*!< Positive number is idx of action parameter,
                   negative number refers to object ID (constant) */
    int arg_size;
    int neg; /*!< True if it is in negative form */
    int func_val; /*!< Assigned value in the case of a function */
};
typedef struct pddl_action_pred pddl_action_pred_t;

struct pddl_action_preds {
    pddl_action_pred_t *pred;
    int size;
    int alloc;
};
typedef struct pddl_action_preds pddl_action_preds_t;


/**
 * Conditional effect
 */
struct pddl_action_cond_eff {
    pddl_action_preds_t pre;
    pddl_action_preds_t eff;
};
typedef struct pddl_action_cond_eff pddl_action_cond_eff_t;

struct pddl_action_cond_effs {
    pddl_action_cond_eff_t *cond_eff;
    int size;
};
typedef struct pddl_action_cond_effs pddl_action_cond_effs_t;


/**
 * Lifted action
 */
struct pddl_action {
    const char *name;
    pddl_action_params_t param;
    pddl_action_preds_t pre;
    pddl_action_preds_t eff;
    pddl_action_preds_t cost;
    pddl_action_cond_effs_t cond_eff;
};
typedef struct pddl_action pddl_action_t;

struct pddl_actions {
    pddl_action_t *action;
    int size;
};
typedef struct pddl_actions pddl_actions_t;


/**
 * Parses actions from domain PDDL.
 */
int pddlActionsParse(const pddl_lisp_t *domain,
                     const pddl_types_t *types,
                     const pddl_objs_t *objs,
                     const pddl_type_obj_t *type_obj,
                     const pddl_predicates_t *predicates,
                     const pddl_predicates_t *functions,
                     unsigned require,
                     pddl_actions_t *actions);

void pddlActionsFree(pddl_actions_t *actions);

pddl_action_t *pddlActionsAdd(pddl_actions_t *as);

void pddlActionPredPrint(const pddl_predicates_t *predicates,
                         const pddl_objs_t *objs,
                         const pddl_action_t *a,
                         const pddl_action_pred_t *f,
                         FILE *fout);

void pddlActionsPrint(const pddl_actions_t *actions,
                      const pddl_objs_t *objs,
                      const pddl_predicates_t *predicates,
                      const pddl_predicates_t *functions,
                      FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_ACTION_H__ */
