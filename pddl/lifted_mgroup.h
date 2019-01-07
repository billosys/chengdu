/***
 * cpddl
 * -------
 * Copyright (c)2019 Daniel Fiser <danfis@danfis.cz>,
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

#ifndef __PDDL_LIFTED_MGROUP_H__
#define __PDDL_LIFTED_MGROUP_H__

#include <pddl/common.h>
#include <pddl/cond.h>
#include <pddl/cond_arr.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_lifted_mgroup {
    pddl_params_t param;
    pddl_cond_arr_t cond;
};
typedef struct pddl_lifted_mgroup pddl_lifted_mgroup_t;

/**
 * Initialize mutex group as a candidate from the given predicate and with
 * (at most) one counted variable specified as the index of the predicate's
 * parameter. If the index is -1 then no variables are counted.
 */
void pddlLiftedMGroupInitCandFromPred(pddl_lifted_mgroup_t *mgroup,
                                      const pddl_pred_t *pred,
                                      int counted_var);

/**
 * Free allocated memory.
 */
void pddlLiftedMGroupFree(pddl_lifted_mgroup_t *mgroup);


/**
 * Returns true if two or more facts from the initial state are covered by
 * the candidate.
 */
int pddlLiftedMGroupIsInitTooHeavy(const pddl_lifted_mgroup_t *cand,
                                   const pddl_t *pddl);

/**
 * Returns true if the action could add two or more facts from the
 * given candidate.
 */
int pddlLiftedMGroupIsActionTooHeavy(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id);

/**
 * Returns true if the action is balanced with respect to the given mutex
 * group candidate, i.e., every add effect covered by the candidate has at
 * least one delete effect \cap precondition covered by the candidate.
 */
int pddlLiftedMGroupIsActionBalanced(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id);


/**
 * Prints a formatted lifted mutex group (or a candidate if there are some
 * counted variables).
 */
void pddlLiftedMGroupPrint(const pddl_t *pddl,
                           const pddl_lifted_mgroup_t *mgroup,
                           FILE *fout);

struct pddl_lifted_mgroups {
    pddl_lifted_mgroup_t *mgroup;
    int mgroup_size;
    int mgroup_alloc;
};
typedef struct pddl_lifted_mgroups pddl_lifted_mgroups_t;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_LIFTED_MGROUP_H__ */
