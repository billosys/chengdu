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

#ifndef __PDDL_STRIPS_OP_H__
#define __PDDL_STRIPS_OP_H__

#include <boruvka/htable.h>
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_strips_op {
    const char *name;
    pddl_fact_id_arr_t pre;
    pddl_fact_id_arr_t del_eff;
    pddl_fact_id_arr_t add_eff;
    int cost;
    // TODO: Conditional effects
};
typedef struct pddl_strips_op pddl_strips_op_t;

void pddlStripsOpInit(pddl_strips_op_t *op);
void pddlStripsOpFree(pddl_strips_op_t *op);

struct pddl_strips_ops {
    pddl_strips_op_t *op;
    int op_size;
    int op_alloc;
    bor_htable_t *htable;
};
typedef struct pddl_strips_ops pddl_strips_ops_t;

void pddlStripsOpsInit(pddl_strips_ops_t *ops);
void pddlStripsOpsFree(pddl_strips_ops_t *ops);
int pddlStripsOpsAdd(pddl_strips_ops_t *ops, const pddl_strips_op_t *add);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_OP_H__ */
