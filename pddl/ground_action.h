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

#ifndef __PDDL_GROUND_ACTION_H__
#define __PDDL_GROUND_ACTION_H__

#include <boruvka/htable.h>
#include <pddl/action.h>
#include <pddl/cond_arr.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;

struct pddl_ground_action {
    const pddl_action_t *action;
    // TODO
    int parent_action; /*!< ID >= 0 if this conditional effect */
    int param_size;
    pddl_cond_arr_t pre_neg;
    pddl_cond_arr_t pre;
    pddl_cond_arr_t add_eff;
    pddl_cond_arr_t del_eff;
    pddl_cond_arr_t assign;
    int max_arg_size;
    int cond_eff_size;
};
typedef struct pddl_ground_action pddl_ground_action_t;

struct pddl_ground_actions {
    pddl_ground_action_t *action;
    int size;
    int alloc;
};
typedef struct pddl_ground_actions pddl_ground_actions_t;

void pddlGroundActionsInit(const struct pddl *pddl, pddl_ground_actions_t *as);
void pddlGroundActionsFree(pddl_ground_actions_t *as);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_GROUND_ACTION_H__ */
