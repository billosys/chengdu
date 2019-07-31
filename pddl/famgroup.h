/***
 * cpddl
 * -------
 * Copyright (c)2018 Daniel Fiser <danfis@danfis.cz>,
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

#ifndef __PDDL_FAMGROUP_H__
#define __PDDL_FAMGROUP_H__

#include <pddl/mgroup.h>
#include <pddl/strips.h>
#include <pddl/sym.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Maximal fam-groups are inferred and also all subsets of those
 *  mutex groups stored in pddl_mgroups_t structure are skipped. */
#define PDDL_FAMGROUP_MAXIMAL 0x1u
/** fam-groups with non-empty intersection with the goal are found */
#define PDDL_FAMGROUP_GOAL 0x2u
/** In the case symmetries are used, only the asymetric fam-groups are
 *  stored in the output set. */
#define PDDL_FAMGROUP_KEEP_ONLY_ASYMETRIC 0x10u
/** Prioritize fam-groups containing new facts */
#define PDDL_FAMGROUP_PRIORITIZE_UNCOVERED 0x20u

/**
 * Find fact-alternating mutex groups while skipping those that are already
 * in mgs. If limit is set to <=0, then no limit on the number of
 * fam-groups is set.
 */
int pddlFAMGroupsInfer(pddl_mgroups_t *mgs,
                       const pddl_strips_t *strips,
                       unsigned int flags,
                       int limit,
                       float time_limit);

int pddlFAMGroupsInferSym(pddl_mgroups_t *mgs,
                          const pddl_strips_t *strips,
                          unsigned int flags,
                          const pddl_strips_sym_t *sym,
                          int limit,
                          float time_limit);

_bor_inline int pddlFAMGroupsInferMaximal(pddl_mgroups_t *mgs,
                                          const pddl_strips_t *strips)
{
    return pddlFAMGroupsInfer(mgs, strips, PDDL_FAMGROUP_MAXIMAL, 0, -1.);
}

_bor_inline int pddlFAMGroupsInferAll(pddl_mgroups_t *mgs,
                                      const pddl_strips_t *strips)
{
    return pddlFAMGroupsInfer(mgs, strips, 0, 0, -1.);
}

/**
 * Find dead-end operators using the fam-groups stored in mgs.
 */
void pddlFAMGroupsDeadEndOps(const pddl_mgroups_t *mgs,
                             const pddl_strips_t *strips,
                             bor_iset_t *dead_end_ops);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FAMGROUP_H__ */
