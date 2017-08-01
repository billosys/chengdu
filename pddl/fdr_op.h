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

#ifndef __PDDL_FDR_OP_H__
#define __PDDL_FDR_OP_H__

#include <pddl/fdr_part_state.h>
#include <pddl/strips_op.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_fdr_cond_eff {
    pddl_fdr_part_state_t pre;
    pddl_fdr_part_state_t eff;
};
typedef struct pddl_fdr_cond_eff pddl_fdr_cond_eff_t;

struct pddl_fdr_op {
    int id;
    int strips_op_id;

    char *name;
    int cost;
    pddl_fdr_part_state_t pre;
    pddl_fdr_part_state_t eff;
    pddl_fdr_cond_eff_t *cond_eff;
    int cond_eff_size;
    int cond_eff_alloc;
};
typedef struct pddl_fdr_op pddl_fdr_op_t;

int pddlFDROpInit(pddl_fdr_op_t *op,
                  int id,
                  const pddl_fdr_vars_t *vars,
                  const pddl_strips_op_t *strips_op);
void pddlFDROpFree(pddl_fdr_op_t *op);

void pddlFDROpPrettyPrint(const pddl_fdr_op_t *op,
                          const pddl_fdr_vars_t *vars,
                          FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FDR_OP_H__ */
