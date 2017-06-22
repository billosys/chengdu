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

#ifndef __PDDL_FDR_VAR_H__
#define __PDDL_FDR_VAR_H__

#include <pddl/pddl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_fdr_val {
    char *name;
    int fact_id;
    int strips_fact_id;
};
typedef struct pddl_fdr_val pddl_fdr_val_t;

struct pddl_fdr_var {
    int range;
    pddl_fdr_val_t *val;
};
typedef struct pddl_fdr_var pddl_fdr_var_t;

struct pddl_fdr_vars {
    pddl_fdr_var_t *var;
    int size;
};
typedef struct pddl_fdr_vars pddl_fdr_vars_t;

void pddlFDRVarsInit(pddl_fdr_vars_t *vars);
void pddlFDRVarsFree(pddl_fdr_vars_t *vars);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FDR_VAR_H__ */
