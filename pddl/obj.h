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

#ifndef __PDDL_OBJ_H__
#define __PDDL_OBJ_H__

#include <pddl/type.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_obj {
    const char *name; /*!< Name of the object */
    int type;         /*!< Type of the object */
    int is_constant;  /*!< True if it is constant (defined in domain) */
    int is_private;   /*!< True if the object is private to an agent */
    int owner;        /*!< -1 or ID of the object corresponding to an agent
                           in unfactored privacy model */
    int is_agent;     /*!< True if the object correspondnds to an agent in
                           unfactored privacy model */
};
typedef struct pddl_obj pddl_obj_t;

struct pddl_objs {
    pddl_obj_t *obj;
    int size;
    int alloc;
};
typedef struct pddl_objs pddl_objs_t;

/**
 * Parse :constants and :objects from domain and problem PDDLs.
 */
int pddlObjsParse(const pddl_lisp_t *domain,
                  const pddl_lisp_t *problem,
                  const pddl_types_t *types,
                  unsigned require,
                  pddl_objs_t *objs);

/**
 * Frees allocated resources.
 */
void pddlObjsFree(pddl_objs_t *objs);

/**
 * Returns ID of the object of the specified name.
 */
int pddlObjsGet(const pddl_objs_t *objs, const char *name);

/**
 * Adds a new obj at the end of the array.
 */
pddl_obj_t *pddlObjsAdd(pddl_objs_t *objs);

/**
 * Print formated objects.
 */
void pddlObjsPrint(const pddl_objs_t *objs, FILE *fout);


/**
 * Mapping between type and objects.
 */
struct pddl_type_obj {
    int **map;
    int *map_size;
    int size;
};
typedef struct pddl_type_obj pddl_type_obj_t;

/**
 * Initializes mapping between types and objects.
 */
int pddlTypeObjInit(pddl_type_obj_t *to,
                    const pddl_types_t *types,
                    const pddl_objs_t *objs);

/**
 * Frees allocated resources
 */
void pddlTypeObjFree(pddl_type_obj_t *to);

/**
 * Returns list of object IDs of a given type.
 */
const int *pddlTypeObjGet(const pddl_type_obj_t *to, int type_id, int *size);

void pddlTypeObjPrint(const pddl_type_obj_t *to, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_OBJ_H__ */
