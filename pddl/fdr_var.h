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

#include <pddl/mgroup.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_fdr_val {
    char *name;
    int var_id; /*!< ID of the variable this value belongs to */
    int val_id; /*!< Value ID within the variable */
    int id; /*!< Global unique ID of this value */
    int strips_fact_id; /*!< ID of the strips fact corresponding to the
                             value, or -1 if the value means "none of those" */
};
typedef struct pddl_fdr_val pddl_fdr_val_t;

struct pddl_fdr_var {
    pddl_fdr_val_t *val;
    int size;
    pddl_fdr_val_t *none_of_those; /*!< "none of those" value or NULL */
};
typedef struct pddl_fdr_var pddl_fdr_var_t;

struct pddl_fdr_vars {
    pddl_fdr_var_t *var;
    int size;

    pddl_fdr_val_t **strips_fact_id_to_val; /*!< Mapping from strips fact_id
                                                 to pddl_fdr_val_t structure */
};
typedef struct pddl_fdr_vars pddl_fdr_vars_t;

#define PDDL_FDR_VARS_ESSENTIAL_FIRST 0u
#define PDDL_FDR_VARS_LARGEST_FIRST 1u
// TODO: Minimazion of bits required for storing the whole state
#define PDDL_FDR_VARS_MIN_BITS

void pddlFDRVarsInit(pddl_fdr_vars_t *vars, const pddl_strips_t *strips,
                     const pddl_mgroups_t *mg, unsigned flags);
void pddlFDRVarsFree(pddl_fdr_vars_t *vars);
void pddlFDRVarsPrint(const pddl_fdr_vars_t *vars, FILE *fout);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FDR_VAR_H__ */
