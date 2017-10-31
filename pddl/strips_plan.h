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

#ifndef __PDDL_STRIPS_PLAN_H__
#define __PDDL_STRIPS_PLAN_H__

#include <boruvka/iset.h>
#include <boruvka/iarr.h>

#include <pddl/strips.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_strips_plan {
    bor_iarr_t plan; /*!< ID of operators in the plan */
    bor_iset_t plan_set; /*!< Set of IDs of operators of the plan */
    int cost;
};
typedef struct pddl_strips_plan pddl_strips_plan_t;

int pddlStripsPlanLoadFromFile(pddl_strips_plan_t *plan,
                               const pddl_strips_t *strips,
                               const char *fn);
void pddlStripsPlanFree(pddl_strips_plan_t *plan);

_bor_inline int pddlStripsPlanLength(const pddl_strips_plan_t *plan)
{
    return borIArrSize(&plan->plan);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_PLAN_H__ */
