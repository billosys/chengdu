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

struct pddl_predicate {
    const char *name;
    int *param;
    int param_size;
    int is_private;
    int owner_param;
};
typedef struct pddl_predicate pddl_predicate_t;

struct pddl_predicates {
    pddl_predicate_t *pred;
    int size;
    int eq_pred;
    int alloc_size;
};
typedef struct pddl_predicates pddl_predicates_t;

/**
 * Parse :predicates from domain PDDL.
 */
int pddlPredicatesParse(const pddl_lisp_t *domain,
                        unsigned require,
                        const pddl_types_t *types,
                        pddl_predicates_t *ps);

/**
 * Parse :functions from domain PDDL.
 */
int pddlFunctionsParse(const pddl_lisp_t *domain,
                       const pddl_types_t *types,
                       pddl_predicates_t *ps);

/**
 * Frees allocated resources.
 */
void pddlPredicatesFree(pddl_predicates_t *ps);

/**
 * Returns ID of the predicate with the specified name.
 */
int pddlPredicatesGet(const pddl_predicates_t *ps, const char *name);

/**
 * Adds a new predicate to the end.
 */
pddl_predicate_t *pddlPredicatesAdd(pddl_predicates_t *ps);

/**
 * Removes last predicate from the array.
 */
void pddlPredicatesRemoveLast(pddl_predicates_t *ps);

void pddlPredicatesPrint(const pddl_predicates_t *ps,
                         const char *title, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_PRED_H__ */
