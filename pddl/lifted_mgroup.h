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

struct pddl_lifted_mgroups {
    pddl_lifted_mgroup_t *mgroup;
    int mgroup_size;
    int mgroup_alloc;
};
typedef struct pddl_lifted_mgroups pddl_lifted_mgroups_t;

#define PDDL_LIFTED_MGROUPS_INIT { 0 }

/**
 * Initialize lifted mgroup as a copy of src.
 */
void pddlLiftedMGroupInitCopy(pddl_lifted_mgroup_t *dst,
                              const pddl_lifted_mgroup_t *src);

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
 * Sort mutex group's atoms and parameters.
 */
void pddlLiftedMGroupSort(pddl_lifted_mgroup_t *m);

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
 * If the test fails and refined != NULL, then a new refined candidates are
 * put in refined.
 */
int pddlLiftedMGroupIsActionBalanced(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id,
                                     pddl_lifted_mgroups_t *refined);


/**
 * Returns true if the conjuction of atoms grounded using given arguments
 * is too heavy, i.e., if the candidate can be unified with at least two
 * atoms.
 */
int pddlLiftedMGroupIsGroundedConjTooHeavy(const pddl_lifted_mgroup_t *mg,
                                           const pddl_cond_arr_t *conj,
                                           const pddl_obj_id_t *conj_args);

int pddlLiftedMGroupsIsGroundedConjTooHeavy(const pddl_lifted_mgroups_t *cand,
                                           const pddl_cond_arr_t *conj,
                                           const pddl_obj_id_t *conj_args);

/**
 * Prints a formatted lifted mutex group (or a candidate if there are some
 * counted variables).
 */
void pddlLiftedMGroupPrint(const pddl_t *pddl,
                           const pddl_lifted_mgroup_t *mgroup,
                           FILE *fout);



/**
 * Initialize empty structure.
 */
void pddlLiftedMGroupsInit(pddl_lifted_mgroups_t *lm);

/**
 * Free allocated memory.
 */
void pddlLiftedMGroupsFree(pddl_lifted_mgroups_t *lm);

/**
 * Adds a copy of the given lifted mgroup.
 */
void pddlLiftedMGroupsAdd(pddl_lifted_mgroups_t *lm,
                          const pddl_lifted_mgroup_t *lmg);

/**
 * Sort mgroups according to size and predicates and removes duplicates.
 */
void pddlLiftedMGroupsSortAndUniq(pddl_lifted_mgroups_t *lm);

/**
 * Find lifted mgroups using "guess, check, refine" approach.
 */
void pddlLiftedMGroupsInfer(const pddl_t *pddl, pddl_lifted_mgroups_t *lm);

void pddlLiftedMGroupsPrint(const pddl_t *pddl,
                            const pddl_lifted_mgroups_t *lm,
                            FILE *fout);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_LIFTED_MGROUP_H__ */
