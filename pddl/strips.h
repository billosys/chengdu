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
#include <pddl/pddl.h>
#include <pddl/strips_op.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Flags: **/
#define PDDL_STRIPS_GROUND_FULL 0x1u
#define PDDL_STRIPS_GROUND_NAIVE 0x2u

struct pddl_strips {
    const pddl_t *pddl;
    pddl_facts_t fact;
    pddl_strips_ops_t op;
    pddl_fact_id_arr_t init;
    pddl_fact_id_arr_t goal;
    int goal_is_unreachable;
};
typedef struct pddl_strips pddl_strips_t;

pddl_strips_t *pddlStripsGround(const pddl_t *pddl, unsigned flags);
void pddlStripsDel(pddl_strips_t *strips);
void pddlStripsDump(const pddl_strips_t *strips, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_H__ */
