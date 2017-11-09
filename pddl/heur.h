
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

#ifndef __PDDL_HEUR_H__
#define __PDDL_HEUR_H__

#include <pddl/strips.h>
#include <pddl/landmark.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int pddlHeurLMCut(const pddl_strips_t *strips,
                  const bor_iset_t *init,
                  const bor_iset_t *goal,
                  pddl_landmarks_t *ldms);

int pddlHeurFlow(const pddl_strips_t *strips,
                 const bor_iset_t *init,
                 const bor_iset_t *goal,
                 const pddl_landmarks_t *ldms);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_HEUR_H__ */
