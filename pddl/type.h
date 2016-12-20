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

#ifndef __PDDL_TYPE_H__
#define __PDDL_TYPE_H__

#include <pddl/lisp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;

struct pddl_type {
    const char *name; /*!< Name of the type */
    int parent;       /*!< ID of the parent type */
};
typedef struct pddl_type pddl_type_t;

struct pddl_types {
    pddl_type_t *type;
    int size;
};
typedef struct pddl_types pddl_types_t;

/**
 * Parses :types into type array.
 */
int pddlTypesParse(struct pddl *pddl);

/**
 * Frees allocated resources.
 */
void pddlTypesFree(pddl_types_t *types);

/**
 * Returns ID of the type corresponding to the name.
 */
int pddlTypesGet(const pddl_types_t *t, const char *name);

/**
 * Prints list of types to the specified output.
 */
void pddlTypesPrint(const pddl_types_t *t, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_TYPE_H__ */
