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

#ifndef __PDDL_PRED_H__
#define __PDDL_PRED_H__

#include <pddl/require.h>
#include <pddl/type.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// TODO: Support for (:either ...) as a type of parameter
struct pddl_pred {
    const char *name; /*!< Name of the predicate */
    int *param;       /*!< IDs of types of parameters */
    int param_size;   /*!< Number of parameters */
    int is_private;   /*!< True if the predicate is private */
    int owner_param;  /*!< Index of the parameter that corresponds to the
                           owner object */
};
typedef struct pddl_pred pddl_pred_t;

struct pddl_preds {
    pddl_pred_t *pred;
    int size;
    int alloc;
    int eq_pred; /*!< -1 or index of the predicate that corresponds to
                      (= .  .) predicate */
};
typedef struct pddl_preds pddl_preds_t;

/**
 * Parse :predicates from domain PDDL.
 */
int pddlPredsParse(const pddl_lisp_t *domain,
                   unsigned require,
                   const pddl_types_t *types,
                   pddl_preds_t *ps);

/**
 * Parse :functions from domain PDDL.
 */
int pddlFunctionsParse(const pddl_lisp_t *domain,
                       const pddl_types_t *types,
                       pddl_preds_t *ps);

/**
 * Frees allocated resources.
 */
void pddlPredsFree(pddl_preds_t *ps);

/**
 * Returns ID of the predicate with the specified name.
 */
int pddlPredsGet(const pddl_preds_t *ps, const char *name);

/**
 * Adds a new predicate to the end.
 */
pddl_pred_t *pddlPredsAdd(pddl_preds_t *ps);

/**
 * Removes last predicate from the array.
 */
void pddlPredsRemoveLast(pddl_preds_t *ps);

void pddlPredsPrint(const pddl_preds_t *ps,
                    const char *title, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_PRED_H__ */
