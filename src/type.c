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
#include "lisp_err.h"
#include "assert.h"

static const char *object_name = "object";

int pddlTypesGet(const pddl_types_t *t, const char *name)
{
    for (int i = 0; i < t->type_size; ++i){
        if (strcmp(t->type[i].name, name) == 0)
            return i;
    }

    return -1;
}


static int add(pddl_types_t *t, const char *name, int parent)
{
    int id;

    if ((id = pddlTypesGet(t, name)) != -1)
        return id;

    ++t->type_size;
    t->type = BOR_REALLOC_ARR(t->type, pddl_type_t, t->type_size);
    t->type[t->type_size - 1].name = name;
    t->type[t->type_size - 1].parent = parent;
    borISetInit(&t->type[t->type_size - 1].child);
    if (parent >= 0)
        borISetAdd(&t->type[parent].child, t->type_size - 1);
    borISetInit(&t->type[t->type_size - 1].either);
    return t->type_size - 1;
}

static int setCB(const pddl_lisp_node_t *root,
                 int child_from, int child_to, int child_type, void *ud,
                 bor_err_t *err)
{
    pddl_types_t *t = ud;
    int pid;

    pid = 0;
    if (child_type >= 0){
        if (root->child[child_type].value == NULL){
            ERR_LISP_RET2(err, -1, root->child + child_type,
                          "Invalid typed list. Unexpected expression");
        }
        pid = add(t, root->child[child_type].value, 0);
    }

    for (int i = child_from; i < child_to; ++i){
        // This is checked in pddlLispParseTypedList()
        ASSERT(root->child[i].value != NULL);
        if (root->child[i].value == NULL)
            ERR_LISP_RET2(err, -1, root->child + i, "Unexpected expression");

        add(t, root->child[i].value, pid);
    }

    return 0;
}

int pddlTypesParse(pddl_t *pddl, bor_err_t *e)
{
    pddl_types_t *types;
    const pddl_lisp_node_t *n;

    // Create a default "object" type
    types = &pddl->type;
    types->type_size = 1;
    types->type = BOR_ALLOC(pddl_type_t);
    types->type[0].name = object_name;
    types->type[0].parent = -1;
    borISetInit(&types->type[0].child);
    borISetInit(&types->type[0].either);

    n = pddlLispFindNode(&pddl->domain_lisp->root, PDDL_KW_TYPES);
    if (n != NULL){
        if (pddlLispParseTypedList(n, 1, n->child_size, setCB, types, e) != 0){
            BOR_TRACE_PREPEND_RET(e, -1, "Invalid definition of :types in %s: ",
                                  pddl->domain_lisp->filename);
        }
    }

    if (types->type_size > 0)
        types->obj_by_type = BOR_CALLOC_ARR(pddl_objs_by_type_t,
                                            types->type_size);

    // TODO: Check circular dependency on types
    return 0;
}

void pddlTypesFree(pddl_types_t *types)
{
    for (int i = 0; i < types->type_size; ++i){
        if (borISetSize(&types->type[i].either) > 0)
            BOR_FREE((char *)types->type[i].name);
        borISetFree(&types->type[i].child);
        borISetFree(&types->type[i].either);
    }

    if (types->type != NULL)
        BOR_FREE(types->type);

    if (types->obj_by_type != NULL){
        for (int i = 0; i < types->type_size; ++i){
            if (types->obj_by_type[i].obj != NULL)
                BOR_FREE(types->obj_by_type[i].obj);
        }
        BOR_FREE(types->obj_by_type);
    }
}

void pddlTypesPrint(const pddl_types_t *t, FILE *fout)
{
    fprintf(fout, "Type[%d]:\n", t->type_size);
    for (int i = 0; i < t->type_size; ++i){
        fprintf(fout, "    [%d]: %s, parent: %d", i,
                t->type[i].name, t->type[i].parent);
        fprintf(fout, "\n");
    }

    fprintf(fout, "Obj-by-Type:\n");
    for (int i = 0; i < t->type_size; ++i){
        fprintf(fout, "    [%d]:", i);
        for (int j = 0; j < t->obj_by_type[i].obj_size; ++j)
            fprintf(fout, " %d", (int)t->obj_by_type[i].obj[j]);
        fprintf(fout, "\n");
    }
}

int pddlTypesIsEither(const pddl_types_t *ts, int tid)
{
    return borISetSize(&ts->type[tid].either) > 0;
}

void pddlTypesAddObj(pddl_types_t *ts, pddl_obj_id_t obj_id, int type_id)
{
    pddl_objs_by_type_t *obj;

    obj = ts->obj_by_type + type_id;
    for (int i = 0; i < obj->obj_size; ++i){
        if (obj->obj[i] == obj_id)
            return;
    }

    if (obj->obj_size >= obj->obj_alloc){
        if (obj->obj_alloc == 0)
            obj->obj_alloc = 2;
        obj->obj_alloc *= 2;
        obj->obj = BOR_REALLOC_ARR(obj->obj, pddl_obj_id_t, obj->obj_alloc);
    }

    obj->obj[obj->obj_size++] = obj_id;

    if (ts->type[type_id].parent != -1)
        pddlTypesAddObj(ts, obj_id, ts->type[type_id].parent);
}

const pddl_obj_id_t *pddlTypesObjsByType(const pddl_types_t *ts, int type_id,
                                         int *size)
{
    if (size != NULL)
        *size = ts->obj_by_type[type_id].obj_size;
    return ts->obj_by_type[type_id].obj;
}

int pddlTypeNumObjs(const pddl_types_t *ts, int type_id)
{
    return ts->obj_by_type[type_id].obj_size;
}

int pddlTypesObjHasType(const pddl_types_t *ts, int type, pddl_obj_id_t obj)
{
    // TODO: can be done in constant time!
    const pddl_obj_id_t *objs;
    int size;

    objs = pddlTypesObjsByType(ts, type, &size);
    for (int i = 0; i < size; ++i){
        if (objs[i] == obj)
            return 1;
    }
    return 0;
}


static int pddlTypesEither(pddl_types_t *ts, const bor_iset_t *either)
{
    pddl_type_t *type;
    pddl_objs_by_type_t *obj;
    char *cur;
    int tid;

    // Try to find already created (either ...) type
    for (int i = 0; i < ts->type_size; ++i){
        if (pddlTypesIsEither(ts, i)
                && borISetEq(&ts->type[i].either, either)){
            return i;
        }
    }

    // Create a new type
    ++ts->type_size;
    ts->type = BOR_REALLOC_ARR(ts->type, pddl_type_t, ts->type_size);
    ts->obj_by_type = BOR_REALLOC_ARR(ts->obj_by_type, pddl_objs_by_type_t,
                                      ts->type_size);

    type = ts->type + ts->type_size - 1;
    type->parent = -1;
    borISetInit(&type->child);
    borISetUnion(&type->child, either);
    borISetInit(&type->either);
    borISetUnion(&type->either, either);

    // Construct a name of the (either ...) type
    int eid;
    int slen = 0;
    BOR_ISET_FOR_EACH(&type->either, eid)
        slen += 1 + strlen(ts->type[eid].name);
    slen += 2 + 6 + 1;
    type->name = cur = BOR_ALLOC_ARR(char, slen);
    cur += sprintf(cur, "(either");
    BOR_ISET_FOR_EACH(&type->either, eid)
        cur += sprintf(cur, " %s", ts->type[eid].name);
    sprintf(cur, ")");
    tid = ts->type_size - 1;

    // Merge obj IDs from all simple types from which this (either ...)
    // type consists of.
    obj = ts->obj_by_type + ts->type_size - 1;
    bzero(obj, sizeof(*obj));
    BOR_ISET_FOR_EACH(&type->either, eid){
        for (int j = 0; j < ts->obj_by_type[eid].obj_size; ++j){
            pddlTypesAddObj(ts, ts->obj_by_type[eid].obj[j], tid);
        }
    }

    return tid;
}


int pddlTypeFromLispNode(pddl_types_t *ts, const pddl_lisp_node_t *node,
                         bor_err_t *err)
{
    int tid;

    if (node->value != NULL){
        tid = pddlTypesGet(ts, node->value);
        if (tid < 0)
            ERR_LISP_RET(err, -1, node, "Unkown type `%s'", node->value);
        return tid;
    }

    if (node->child_size < 2 || node->child[0].kw != PDDL_KW_EITHER)
        ERR_LISP_RET2(err, -1, node, "Unknown expression");

    if (node->child_size == 2 && node->child[1].value != NULL)
        return pddlTypeFromLispNode(ts, node->child + 1, err);

    BOR_ISET(either);
    for (int i = 1; i < node->child_size; ++i){
        if (node->child[i].value == NULL){
            ERR_LISP_RET2(err, -1, node->child + i,
                          "Invalid (either ...) expression");
        }
        tid = pddlTypesGet(ts, node->child[i].value);
        if (tid < 0){
            ERR_LISP_RET(err, -1, node->child + i, "Unkown type `%s'",
                         node->child[i].value);
        }

        borISetAdd(&either, tid);
    }

    tid = pddlTypesEither(ts, &either);
    borISetFree(&either);
    return tid;
}

int pddlTypesIsParent(const pddl_types_t *ts, int child, int parent)
{
    const pddl_type_t *tparent = ts->type + parent;
    int eid;

    for (int cur_type = child; cur_type >= 0;){
        if (cur_type == parent)
            return 1;
        BOR_ISET_FOR_EACH(&tparent->either, eid){
            if (cur_type == eid)
                return 1;
        }
        cur_type = ts->type[cur_type].parent;
    }

    return 0;
}

int pddlTypesAreDisjunct(const pddl_types_t *ts, int t1, int t2)
{
    return !pddlTypesIsParent(ts, t1, t2) && !pddlTypesIsParent(ts, t2, t1);
}

void pddlTypesPrintPDDL(const pddl_types_t *ts, FILE *fout)
{
    int q[ts->type_size];
    int qi = 0, qsize = 0;

    fprintf(fout, "(:types\n");
    for (int i = 0; i < ts->type_size; ++i){
        if (ts->type[i].parent == 0)
            q[qsize++] = i;
    }

    for (qi = 0; qi < qsize; ++qi){
        fprintf(fout, "    %s - %s\n",
                ts->type[q[qi]].name,
                ts->type[ts->type[q[qi]].parent].name);
        for (int i = 0; i < ts->type_size; ++i){
            if (ts->type[i].parent == q[qi] && !pddlTypesIsEither(ts, i))
                q[qsize++] = i;
        }
    }

    fprintf(fout, ")\n");
}
