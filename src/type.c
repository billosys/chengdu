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
#include "assert.h"

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
    t->type[t->size - 1].either = NULL;
    t->type[t->size - 1].either_size = 0;
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
            ERR_LISP_RET2(-1, root->child + child_type,
                          "Invalid typed list. Unexpected expression");
        }
        pid = add(t, root->child[child_type].value);
    }

    for (i = child_from; i < child_to; ++i){
        // This is checked in pddlLispParseTypedList()
        ASSERT(root->child[i].value != NULL);
        if (root->child[i].value == NULL)
            ERR_LISP_RET2(-1, root->child + i, "Unexpected expression");

        tid = add(t, root->child[i].value);
        if (tid != 0)
            t->type[tid].parent = pid;
    }

    return 0;
}

int pddlTypesParse(pddl_t *pddl)
{
    pddl_types_t *types;
    const pddl_lisp_node_t *n;

    // Create a default "object" type
    types = &pddl->type;
    types->size = 1;
    types->type = BOR_ALLOC(pddl_type_t);
    types->type[0].name = object_name;
    types->type[0].parent = -1;
    types->type[0].either = NULL;
    types->type[0].either_size = 0;

    n = pddlLispFindNode(&pddl->domain_lisp->root, PDDL_KW_TYPES);
    if (n != NULL){
        if (pddlLispParseTypedList(n, 1, n->child_size, setCB, types) != 0){
            TRACE_UPDATE_RET(-1, "Invalid definition of :types in %s: ",
                             pddl->domain_lisp->filename);
        }
    }

    if (types->size > 0)
        types->obj_by_type = BOR_CALLOC_ARR(pddl_objs_by_type_t, types->size);

    // TODO: Check circular dependency on types
    return 0;
}

void pddlTypesFree(pddl_types_t *types)
{
    int i;

    for (i = 0; i < types->size; ++i){
        if (types->type[i].either != NULL){
            BOR_FREE((char *)types->type[i].name);
            BOR_FREE(types->type[i].either);
        }
    }

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
    int i;

    obj = ts->obj_by_type + type_id;
    for (i = 0; i < obj->size; ++i){
        if (obj->obj[i] == obj_id)
            return;
    }

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

int pddlTypesObjHasType(const pddl_types_t *ts, int type, int obj)
{
    // TODO: can be done in constant time!
    const int *objs;
    int size, i;

    objs = pddlTypesObjsByType(ts, type, &size);
    for (i = 0; i < size; ++i){
        if (objs[i] == obj)
            return 1;
    }
    return 0;
}


static int pddlTypesEither(pddl_types_t *ts, const int *either, int either_size)
{
    pddl_type_t *type;
    pddl_objs_by_type_t *obj;
    char *cur;
    int i, j, slen;
    int tid;

    // Try to find already created (either ...) type
    for (i = 0; i < ts->size; ++i){
        if (ts->type[i].either == NULL)
            continue;
        if (ts->type[i].either_size == either_size
                && memcmp(ts->type[i].either, either,
                          sizeof(int) * either_size) == 0){
            return i;
        }
    }

    // Create a new type
    ++ts->size;
    ts->type = BOR_REALLOC_ARR(ts->type, pddl_type_t, ts->size);
    ts->obj_by_type = BOR_REALLOC_ARR(ts->obj_by_type, pddl_objs_by_type_t,
                                      ts->size);

    type = ts->type + ts->size - 1;
    type->parent = -1;
    type->either = BOR_ALLOC_ARR(int, either_size);
    memcpy(type->either, either, sizeof(int) * either_size);
    type->either_size = either_size;

    // Construct a name of the (either ...) type
    for (i = 0, slen = 0; i < either_size; ++i)
        slen += 1 + strlen(ts->type[either[i]].name);
    slen += 2 + 6 + 1;
    type->name = cur = BOR_ALLOC_ARR(char, slen);
    cur += sprintf(cur, "(either");
    for (i = 0; i < either_size; ++i)
        cur += sprintf(cur, " %s", ts->type[either[i]].name);
    sprintf(cur, ")");
    tid = ts->size - 1;

    // Merge obj IDs from all simple types from which this (either ...)
    // type consists of.
    obj = ts->obj_by_type + ts->size - 1;
    bzero(obj, sizeof(*obj));
    for (i = 0; i < either_size; ++i){
        for (j = 0; j < ts->obj_by_type[either[i]].size; ++j){
            pddlTypesAddObj(ts, ts->obj_by_type[either[i]].obj[j], tid);
        }
    }

    return tid;
}


static int cmpInt(const void *a, const void *b)
{
    int ia = *(int *)a;
    int ib = *(int *)b;
    return ia - ib;
}

int pddlTypeFromLispNode(pddl_types_t *ts, const pddl_lisp_node_t *node)
{
    int *either;
    int either_size;
    int i, tid;

    if (node->value != NULL){
        tid = pddlTypesGet(ts, node->value);
        if (tid < 0)
            ERR_LISP_RET(-1, node, "Unkown type `%s'", node->value);
        return tid;
    }

    if (node->child_size < 2 || node->child[0].kw != PDDL_KW_EITHER)
        ERR_LISP_RET2(-1, node, "Unknown expression");

    if (node->child_size == 2 && node->child[1].value != NULL)
        return pddlTypeFromLispNode(ts, node->child + 1);

    either_size = node->child_size - 1;
    either = alloca(sizeof(int) * either_size);
    for (i = 1; i < node->child_size; ++i){
        if (node->child[i].value == NULL){
            ERR_LISP_RET2(-1, node->child + i,
                          "Invalid (either ...) expression");
        }
        tid = pddlTypesGet(ts, node->child[i].value);
        if (tid < 0){
            ERR_LISP_RET(-1, node->child + i, "Unkown type `%s'",
                         node->child[i].value);
        }

        either[i - 1] = tid;
    }

    qsort(either, either_size, sizeof(int), cmpInt);

    return pddlTypesEither(ts, either, either_size);
}

void pddlTypesPrintPDDL(const pddl_types_t *ts, FILE *fout)
{
    int q[ts->size];
    int qi = 0, qsize = 0;

    fprintf(fout, "(:types\n");
    for (int i = 0; i < ts->size; ++i){
        if (ts->type[i].parent == 0)
            q[qsize++] = i;
    }

    for (qi = 0; qi < qsize; ++qi){
        fprintf(fout, "    %s - %s\n",
                ts->type[q[qi]].name,
                ts->type[ts->type[q[qi]].parent].name);
        for (int i = 0; i < ts->size; ++i){
            if (ts->type[i].parent == q[qi] && ts->type[i].either == NULL)
                q[qsize++] = i;
        }
    }

    fprintf(fout, ")\n");
}
