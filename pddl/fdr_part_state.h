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

#ifndef __PDDL_FDR_PART_STATE_H__
#define __PDDL_FDR_PART_STATE_H__

#include <pddl/fdr_var.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PDDL_FDR_VAL_UNDEF -1

struct pddl_fdr_part_state {
    int *val;
    int size;
};
typedef struct pddl_fdr_part_state pddl_fdr_part_state_t;

void pddlFDRPartStateInit(pddl_fdr_part_state_t *s,
                          const pddl_fdr_vars_t *vars);
void pddlFDRPartStateFree(pddl_fdr_part_state_t *s);
int pddlFDRPartStateSet(pddl_fdr_part_state_t *s, int var, int val);
int pddlFDRPartStateIsSet(const pddl_fdr_part_state_t *s, int var);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FDR_PART_STATE_H__ */
