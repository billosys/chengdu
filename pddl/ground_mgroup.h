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

#ifndef __PDDL_GROUND_MGROUP_H__
#define __PDDL_GROUND_MGROUP_H__

#include <boruvka/iset.h>
#include <pddl/lifted_mgroup.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_ground_mgroup {
    bor_iset_t mgroup; /*!< Set of facts forming the mutex group */
    int lifted_mgroup_id; /*!< ID refering to the corresponding lifted
                               mutex group in pddl_ground_mgroups_t */
};
typedef struct pddl_ground_mgroup pddl_ground_mgroup_t;

struct pddl_ground_mgroups {
    pddl_lifted_mgroups_t lifted_mgroup;
    pddl_ground_mgroup_t *mgroup;
    int mgroup_size;
    int mgroup_alloc;
};
typedef struct pddl_ground_mgroups pddl_ground_mgroups_t;

/**
 * Ground lifted mutex groups using reachable facts.
 */
void pddlGroundMGroupsGround(pddl_ground_mgroups_t *mg,
                             const pddl_t *pddl,
                             const pddl_lifted_mgroups_t *lifted_mg,
                             const pddl_strips_t *strips);

/**
 * Free allocated memory.
 */
void pddlGroundMGroupsFree(pddl_ground_mgroups_t *mg);

/**
 * Debug print out
 */
void pddlGroundMGroupsPrint(const pddl_t *pddl,
                            const pddl_strips_t *strips,
                            const pddl_ground_mgroups_t *mg,
                            FILE *fout);
void pddlGroundMGroupPrint(const pddl_t *pddl,
                           const pddl_strips_t *strips,
                           const pddl_ground_mgroup_t *mg,
                           FILE *fout);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_GROUND_MGROUP_H__ */
