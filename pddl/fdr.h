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

#ifndef __PDDL_FDR_H__
#define __PDDL_FDR_H__

#include <pddl/strips.h>
#include <pddl/fdr_var.h>
#include <pddl/fdr_op.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_fdr {
    const pddl_strips_t *strips; /*!< STRIPS from which was FDR created */
    pddl_fdr_vars_t var; /*!< Variables */
    pddl_fdr_op_t *op; /*!< FDR operators */
    int op_size; /*!< Number of operators */
    pddl_fdr_part_state_t init; /*!< The initial state */
    pddl_fdr_part_state_t goal; /*!< The goal specification */
};
typedef struct pddl_fdr pddl_fdr_t;


pddl_fdr_t *pddlFDRFromStrips(const pddl_strips_t *strips,
                              const pddl_mgroups_t *mgroups,
                              unsigned vars_flags);
void pddlFDRDel(pddl_fdr_t *fdr);

/**
 * Prints FDR problem in Fast Downward's format
 * (see http://www.fast-downward.org/TranslatorOutputFormat).
 */
void pddlFDRPrintAsFD(const pddl_fdr_t *fdr, FILE *fout);

void pddlFDRPrettyPrint(const pddl_fdr_t *fdr, FILE *fout);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FDR_H__ */
