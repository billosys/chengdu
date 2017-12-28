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

#ifndef __PDDL_MGROUP_H__
#define __PDDL_MGROUP_H__

#include <boruvka/iset.h>

#include <pddl/common.h>
#include <pddl/fact.h>
#include <pddl/g.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Mutex group -- at most one fact from the set can be part of any
 * reachable state.
 */
struct pddl_mgroup {
    bor_iset_t fact; /*!< Set of facts the mutex group consists of */
    int none_of_those; /*!< ID of the "none-of-those" fact or -1 */
    int is_init; /*!< True if it has non-empty intersection with the init */
    int is_goal; /*!< True if it has non-empty intersection with the goal */
    int is_fa; /*!< True if it is fact-alternatig mutex group */

    int is_exactly_1; /*!< True if exactly one fact must be true in any
                           reachable state. */
};
typedef struct pddl_mgroup pddl_mgroup_t;

struct pddl_mgroups {
    pddl_mgroup_t *mgroup;
    int mgroup_size;
    int mgroup_alloc;
};
typedef struct pddl_mgroups pddl_mgroups_t;

#define PDDL_MGROUPS_FOR_EACH(MS, M) \
    for (int __i = 0; __i < (MS)->mgroup_size \
                        && ((M) = (MS)->mgroup + __i); ++__i)

void pddlMGroupInit(pddl_mgroup_t *mg);
void pddlMGroupFree(pddl_mgroup_t *mg);

/**
 * Fills in tg a transition graph of the given mutex group.
 */
void pddlMGroupTGInit(pddl_g_t *tg,
                      const pddl_mgroup_t *mg,
                      const pddl_strips_t *strips);

/**
 * Computes synchronized product of two transition graphs.
 */
void pddlMGroupTGSyncProduct(pddl_g_t *prod,
                             const pddl_g_t *tg1,
                             const pddl_g_t *tg2,
                             const pddl_strips_t *strips);

/**
 * Initialize a set of mutex groups.
 */
void pddlMGroupsInit(pddl_mgroups_t *mgs);
pddl_mgroups_t *pddlMGroupsNew(void);

/**
 * Free allocated memory.
 */
void pddlMGroupsFree(pddl_mgroups_t *mgs);
void pddlMGroupsDel(pddl_mgroups_t *mgs);

/**
 * Copy the mgroups structure.
 */
void pddlMGroupsCopy(pddl_mgroups_t *dst, const pddl_mgroups_t *src);

/**
 * Add a mutex group consisting of the given facts.
 */
pddl_mgroup_t *pddlMGroupsAdd(pddl_mgroups_t *mgs, const bor_iset_t *mg);

/**
 * Find fact-alternating mutex groups in the provided strips problem and
 * add them into mgs.
 * The function refuses to work on problems with conditional effects, but
 * conditional effects can be compiled away. Also, this function requires
 * LP solver to be built-in.
 */
int pddlMGroupsFA(const pddl_strips_t *strips, pddl_mgroups_t *mgs);

/**
 * Finalize mutex groups structure.
 * Call this function to fill in all the remaining members besides .fact,
 * .is_fa, .is_init, .is_goal.
 */
void pddlMGroupsFinalize(pddl_mgroups_t *mgs, const pddl_strips_t *strips);

void pddlMGroupsPrettyPrint(const pddl_t *pddl, const pddl_facts_t *fs,
                            const pddl_mgroups_t *ms, FILE *fout);
void pddlMGroupsPrintPython(const pddl_mgroups_t *mg, FILE *fout);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_MGROUP_H__ */
