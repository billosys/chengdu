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

#ifndef __PDDL_LANDMARK_H__
#define __PDDL_LANDMARK_H__

#include <boruvka/iset.h>
#include <pddl/common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_disjunctive_landmarks {
    bor_iset_t *ldm;
    int ldm_size;
    int ldm_alloc;
};
typedef struct pddl_disjunctive_landmarks pddl_disjunctive_landmarks_t;

void pddlDisjunctiveLandmarksInit(pddl_disjunctive_landmarks_t *ldms);
void pddlDisjunctiveLandmarksFree(pddl_disjunctive_landmarks_t *ldms);
void pddlDisjunctiveLandmarksAdd(pddl_disjunctive_landmarks_t *ldms,
                                 const bor_iset_t *ldm);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_LANDMARK_H__ */
