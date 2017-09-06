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

#include <pddl/common.h>
#include <pddl/lisp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_type {
    const char *name; /*!< Name of the type */
    int parent;       /*!< ID of the parent type */

    int *either;      /*!< NULL for normal type, a list type IDs for
                           special (either ...) type */
    int either_size;
};
typedef struct pddl_type pddl_type_t;

struct pddl_objs_by_type {
    int *obj;
    int size;
    int alloc;
};
typedef struct pddl_objs_by_type pddl_objs_by_type_t;

struct pddl_types {
    pddl_type_t *type;
    int size;

    pddl_objs_by_type_t *obj_by_type;
};
typedef struct pddl_types pddl_types_t;

/**
 * Parses :types into type array.
 */
int pddlTypesParse(pddl_t *pddl);

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

/**
 * Record the given object as being of the given type.
 */
void pddlTypesAddObj(pddl_types_t *ts, int obj_id, int type_id);

/**
 * Returns list of object IDs of the specified type.
 */
const int *pddlTypesObjsByType(const pddl_types_t *ts, int type_id, int *size);

/**
 * Returns true if the object compatible with the specified type.
 */
int pddlTypesObjHasType(const pddl_types_t *ts, int type, int obj);

/**
 * Returns type ID from the lisp node or -1 if error occured.
 * (either ...) types are created if necessary.
 */
int pddlTypeFromLispNode(pddl_types_t *ts, const pddl_lisp_node_t *node);


/**
 * Print requirements in PDDL format.
 */
void pddlTypesPrintPDDL(const pddl_types_t *ts, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_TYPE_H__ */
