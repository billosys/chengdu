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
#include <pddl/strips.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Mutex group -- at most one fact from the set can be part of any
 * reachable state.
 */
struct pddl_mgroup {
    bor_iset_t fact; /*!< Set of facts the mutex group consists of */
    int is_init; /*!< True if it has non-empty intersection with the init */
    int is_goal; /*!< True if it has non-empty intersection with the goal */
    int is_fa; /*!< True if it is fact-alternatig mutex group */
};
typedef struct pddl_mgroup pddl_mgroup_t;

struct pddl_mgroups {
    pddl_mgroup_t *g;
    int size;
    int alloc;
};
typedef struct pddl_mgroups pddl_mgroups_t;

void pddlMGroupInit(pddl_mgroup_t *mg);
void pddlMGroupFree(pddl_mgroup_t *mg);

pddl_mgroups_t *pddlMGroupsNew(void);
void pddlMGroupsDel(pddl_mgroups_t *mgs);
pddl_mgroup_t *pddlMGroupsAdd(pddl_mgroups_t *mgs, const bor_iset_t *mg);

pddl_mgroups_t *pddlMGroupFindFA(const pddl_strips_t *strips);

void pddlMGroupsPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                            const pddl_mgroups_t *ms, FILE *fout);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_MGROUP_H__ */
