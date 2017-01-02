/***
 * cpddl
 * -------
 * Copyright (c)2016 Daniel Fiser <danfis@danfis.cz>,
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

#include <pddl/pddl.h>
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Flags: **/
#define PDDL_STRIPS_GROUND_NAIVE 0x1u

struct pddl_strips_op {
    char *name;
    pddl_facts_t pre;
    pddl_facts_t del_eff;
    pddl_facts_t add_eff;
    int cost;
    // TODO: Conditional effects
};
typedef struct pddl_strips_op pddl_strips_op_t;

struct pddl_strips_ops {
    pddl_strips_op_t *op;
    int size;
    int alloc;
};
typedef struct pddl_strips_ops pddl_strips_ops_t;


struct pddl_strips {
    pddl_facts_t fact;
    pddl_strips_ops_t op;
    pddl_facts_t init;
    pddl_facts_t goal;
};
typedef struct pddl_strips pddl_strips_t;

int pddlStripsFromPDDL(pddl_strips_t *strips, const pddl_t *pddl,
                       unsigned flags);
void pddlStripsFree(pddl_strips_t *strips);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_H__ */
