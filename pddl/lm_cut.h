/***
 * cpddl
 * -------
 * Copyright (c)2017 Daniel Fiser <danfis@danfis.cz>,
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

#ifndef __PDDL_LM_CUT_H__
#define __PDDL_LM_CUT_H__

#include <boruvka/iset.h>
#include <boruvka/apq.h>

#include <pddl/common.h>
#include <pddl/strips.h>
#include <pddl/landmark.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CUT_UNDEF 0
#define CUT_INIT 1
#define CUT_GOAL 2

struct pddl_lm_cut_op {
    bor_iset_t pre;
    bor_iset_t eff;
    int op_cost;

    int cost;          /*!< Current cost of the operator */
    int unsat;         /*!< Number of unsatisfied preconditions */
    int supp;          /*!< Supporter fact (that maximizes h^max) */
    int supp_cost;     /*!< Cost of the supported -- needed for hMaxInc() */
    int cut_candidate; /*!< True if the operator is candidate for a cut */
};
typedef struct pddl_lm_cut_op pddl_lm_cut_op_t;

struct pddl_lm_cut_fact {
    bor_iset_t pre_op; /*!< Operators having this fact as its precond */
    bor_iset_t eff_op; /*!< Operators having this fact as its effect */
    int value;
    bor_apq_el_t pq; /*!< Connection to priority queue */
    int supp_cnt;    /*!< Number of operators that have this fact as
                          a supporter. */
    int cut_state; /*!< One of CUT_* */
};
typedef struct pddl_lm_cut_fact pddl_lm_cut_fact_t;

struct pddl_lm_cut {
    pddl_lm_cut_fact_t *fact;
    int fact_size;
    int fact_goal; /*!< ID of the artificial goal fact */
    int fact_nopre; /*!< ID of the artifical fact meaning "no precondition" */

    pddl_lm_cut_op_t *op;
    int op_size;
    int op_goal; /*!< ID of the artificial goal operator */

    bor_iset_t state; /*!< Current state from which heur is computed */
    bor_iset_t cut; /*!< Current cut */
    pddl_disjunctive_landmarks_t ldms;

    /** Auxiliary structures to avoid re-allocation */
    int *queue;
    int queue_size;
    bor_apq_t pq;
};
typedef struct pddl_lm_cut pddl_lm_cut_t;

void pddlLMCutInit(pddl_lm_cut_t *lmcut, const pddl_strips_t *strips);
void pddlLMCutFree(pddl_lm_cut_t *lmcut);

int pddlLMCut(pddl_lm_cut_t *lmcut,
              const bor_iset_t *init, const bor_iset_t *goal);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_LM_CUT_H__ */
