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

#ifndef __PDDL_STRIPS_H__
#define __PDDL_STRIPS_H__

#include <boruvka/htable.h>
#include <boruvka/iset.h>

#include <pddl/strips_op.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;

struct pddl_strips_prune_config {
    /** Run pruning until fixpoint. If set to false, the pruning is run
     *  only once. Default: 1 */
    int fixpoint;

    /** Removing static facts.  Default: 1 */
    int static_facts;

    /** Backward irrelevance analysis. Default: 1 */
    int irrelevance;

    /** Use delete effects in the irrelevance analysis. This is
     *  automatically enabled in the case of problems with conditional
     *  effects. If this option is disabled, the operators without add
     *  effects are automatically removed. This may or may not be desired,
     *  because such a operator cannot be part of the strictly optimal
     *  plan, but it may reduce branching for the satisficing planning.
     *  Default: 0 */
    int irrelevance_del_eff;

    /** Compute h^m mutexes. 0 means no h^m mutexes, 1 means reachability
     *  analysis, 2 means h^2, etc... Default: 2 */
    int h_mutex;

    /** h^m mutexes in regression. Default: 2 */
    int h_mutex_bw;

    /** Compute fact-alternating mutex groups and use it for removal of
     *  unreachable operators. Default: 1 */
    int fa_mgroup;

    /** Use fam-groups for removal of dead-end operators. Default: 1 */
    int fa_mgroup_dead_end;

    /** TODO */
    int disambiguation;
};
typedef struct pddl_strips_prune_config pddl_strips_prune_config_t;

#define PDDL_STRIPS_PRUNE_CONFIG_INIT { \
        1, /* fixpoint */ \
        1, /* static_facts */ \
        1, /* irrelevance */ \
        0, /* irrelevance_del_eff */ \
        2, /* h_mutex */ \
        2, /* h_mutex_bw */ \
        1, /* fa_mgroup */ \
        1, /* fa_mgroup_dead_end */ \
        1, /* disambiguation */ \
    }

struct pddl_strips {
    const struct pddl *pddl;
    pddl_facts_t fact; /*!< Set of facts */
    pddl_strips_ops_t op; /*!< Set of operators */
    bor_iset_t init; /*!< Initial state */
    bor_iset_t goal; /*!< Goal specification */
    int goal_is_unreachable; /*!< True if the goal is not reachable */
    int has_cond_eff; /*!< True if the problem contains operators with
                           conditinal effects. */
};
typedef struct pddl_strips pddl_strips_t;

/**
 * Grounds pddl into strips.
 */
pddl_strips_t *pddlStripsGround(const struct pddl *pddl);

/**
 * Deletes allocated memory.
 */
void pddlStripsDel(pddl_strips_t *strips);

/**
 * Construct a dual strips problem.
 */
pddl_strips_t *pddlStripsDual(const pddl_strips_t *strips);

/**
 * Compile out conditional effects by making each conditional effect a
 * separate operator, i.e., the compilation is linear.
 * This transformation can be useful for a reachability analysis.
 */
pddl_strips_t *pddlStripsCompileAwayCondEffRelaxed(const pddl_strips_t *strips);

/**
 * Prune STRIPS planning task according to the configuration.
 */
int pddlStripsPrune(pddl_strips_t *strips,
                    const pddl_strips_prune_config_t *cfg);

void pddlStripsMakeUnsolvable(pddl_strips_t *strips);

// TODO: pddlStripsPrintPDDLDomain/Problem() --> prints PDDL domain/problem
//       files corresponding to the STRIPS problem
void pddlStripsDump(const pddl_strips_t *strips, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_H__ */
