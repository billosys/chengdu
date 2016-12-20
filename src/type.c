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

#include <boruvka/alloc.h>
#include "pddl/pddl.h"
#include "pddl/type.h"
#include "err.h"

static const char *object_name = "object";

int pddlTypesGet(const pddl_types_t *t, const char *name)
{
    int i;

    for (i = 0; i < t->size; ++i){
        if (strcmp(t->type[i].name, name) == 0)
            return i;
    }

    return -1;
}


static int add(pddl_types_t *t, const char *name)
{
    int id;

    if ((id = pddlTypesGet(t, name)) != -1)
        return id;

    ++t->size;
    t->type = BOR_REALLOC_ARR(t->type, pddl_type_t, t->size);
    t->type[t->size - 1].name = name;
    t->type[t->size - 1].parent = 0;
    return t->size - 1;
}

static int setCB(const pddl_lisp_node_t *root,
                 int child_from, int child_to, int child_type, void *ud)
{
    pddl_types_t *t = ud;
    int i, tid, pid;

    pid = 0;
    if (child_type >= 0){
        if (root->child[child_type].value == NULL){
            ERRN2(root->child + child_type, "Invalid type definition.");
            return -1;
        }
        pid = add(t, root->child[child_type].value);
    }

    for (i = child_from; i < child_to; ++i){
        if (root->child[i].value == NULL){
            ERRN2(root->child + i, "Invalid type definition.");
            return -1;
        }

        tid = add(t, root->child[i].value);
        if (tid != 0)
            t->type[tid].parent = pid;
    }

    return 0;
}

int pddlTypesParse(pddl_t *pddl)
{
    const pddl_lisp_node_t *n;

    // Create a default "object" type
    pddl->type.size = 1;
    pddl->type.type = BOR_ALLOC(pddl_type_t);
    pddl->type.type[0].name = object_name;
    pddl->type.type[0].parent = -1;

    n = pddlLispFindNode(&pddl->domain_lisp->root, PDDL_KW_TYPES);
    if (n == NULL)
        return 0;

    if (pddlLispParseTypedList(n, 1, n->child_size, setCB, &pddl->type) != 0){
        ERRN2(n, "Invalid definition of :types");
        return -1;
    }

    if (pddl->type.size > 0)
        pddl->type.obj_by_type = BOR_CALLOC_ARR(pddl_objs_by_type_t, pddl->type.size);

    // TODO: Check circular dependency on types
    return 0;
}

void pddlTypesFree(pddl_types_t *types)
{
    int i;

    if (types->type != NULL)
        BOR_FREE(types->type);

    if (types->obj_by_type != NULL){
        for (i = 0; i < types->size; ++i){
            if (types->obj_by_type[i].obj != NULL)
                BOR_FREE(types->obj_by_type[i].obj);
        }
        BOR_FREE(types->obj_by_type);
    }
}

void pddlTypesPrint(const pddl_types_t *t, FILE *fout)
{
    int i, j;

    fprintf(fout, "Type[%d]:\n", t->size);
    for (i = 0; i < t->size; ++i){
        fprintf(fout, "    [%d]: %s, parent: %d\n", i,
                t->type[i].name, t->type[i].parent);
    }

    fprintf(fout, "Obj-by-Type:\n");
    for (i = 0; i < t->size; ++i){
        fprintf(fout, "    [%d]:", i);
        for (j = 0; j < t->obj_by_type[i].size; ++j)
            fprintf(fout, " %d", t->obj_by_type[i].obj[j]);
        fprintf(fout, "\n");
    }
}

void pddlTypesAddObj(pddl_types_t *ts, int obj_id, int type_id)
{
    pddl_objs_by_type_t *obj;

    obj = ts->obj_by_type + type_id;
    if (obj->size >= obj->alloc){
        if (obj->alloc == 0)
            obj->alloc = 2;
        obj->alloc *= 2;
        obj->obj = BOR_REALLOC_ARR(obj->obj, int, obj->alloc);
    }

    obj->obj[obj->size++] = obj_id;

    if (ts->type[type_id].parent != -1)
        pddlTypesAddObj(ts, obj_id, ts->type[type_id].parent);
}

const int *pddlTypesObjsByType(const pddl_types_t *ts, int type_id, int *size)
{
    if (size != NULL)
        *size = ts->obj_by_type[type_id].size;
    return ts->obj_by_type[type_id].obj;
}
