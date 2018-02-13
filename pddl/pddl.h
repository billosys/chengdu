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

#ifndef __PDDL_H__
#define __PDDL_H__

#include <pddl/config.h>
#include <pddl/common.h>
#include <pddl/lisp.h>
#include <pddl/require.h>
#include <pddl/type.h>
#include <pddl/obj.h>
#include <pddl/pred.h>
#include <pddl/fact.h>
#include <pddl/action.h>
#include <pddl/strips.h>
#include <pddl/fdr.h>
#include <pddl/mutex.h>
#include <pddl/mgroup.h>
#include <pddl/err.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_config {
    int force_adl; /*!< Force ADL to requirements */
    int normalize; /*!< Normalize pddl, i.e., make preconditions and
                        effects CNF */
    int compile_away_cond_eff; /*!< Compile away conditional effects --
                                    requires .normalize set to true*/

    int strips; /*!< True if the problem should be grounded to STRIPS.
                     Requires normalization turned on. */
    pddl_strips_config_t strips_cfg;

    int fdr; /*!< True if the STRIPS should be translated into FDR */
    unsigned fdr_vars_flags; /*!< See PDDL_FR_VARS_* */
};
typedef struct pddl_config pddl_config_t;

#define PDDL_CONFIG_INIT_EMPTY { 0 }
#define PDDL_CONFIG_INIT \
    { 0, /* force_adl */ \
      1, /* normalize */ \
      0, /* compile_away_cond_eff */ \
      \
      0, /* strips */ \
      PDDL_STRIPS_CONFIG_INIT, /* strips_cfg */ \
      \
      0, /* fdr */ \
      PDDL_FDR_VARS_ESSENTIAL_FIRST, /* fdr_vars_flags */ \
    }

struct pddl {
    pddl_config_t cfg;
    pddl_lisp_t *domain_lisp;
    pddl_lisp_t *problem_lisp;
    const char *domain_name;
    const char *problem_name;
    unsigned require;
    pddl_types_t type;
    pddl_objs_t obj;
    pddl_preds_t pred;
    pddl_preds_t func;
    pddl_cond_part_t *init;
    pddl_cond_t *goal;
    pddl_actions_t action;
    int metric;

    pddl_strips_t *strips;
    pddl_fdr_t *fdr;
};

pddl_t *pddlNew(const char *domain_fn, const char *problem_fn,
                const pddl_config_t *cfg);
void pddlDel(pddl_t *pddl);

void pddlNormalize(pddl_t *pddl);
void pddlCompileAwayCondEff(pddl_t *pddl);

/**
 * Returns maximal number of parameters of all predicates and functions.
 */
int pddlPredFuncMaxParamSize(const pddl_t *pddl);

/**
 * Prints PDDL domain file.
 */
void pddlPrintPDDLDomain(const pddl_t *pddl, FILE *fout);

/**
 * Prints PDDL problem file.
 */
void pddlPrintPDDLProblem(const pddl_t *pddl, FILE *fout);

void pddlPrintDebug(const pddl_t *pddl, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_H__ */
