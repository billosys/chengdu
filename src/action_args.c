/***
 * cpddl
 * -------
 * Copyright (c)2019 Daniel Fiser <danfis@danfis.cz>,
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

#include <boruvka/hfunc.h>
#include "pddl/action_args.h"

struct el {
    int id;
    bor_htable_key_t key;
    bor_list_t htable;
    pddl_obj_id_t args[];
};
typedef struct el el_t;

static bor_htable_key_t hash(const pddl_obj_id_t *args, int size)
{
    return borCityHash_64(args, sizeof(pddl_obj_id_t) * size);
}

static bor_htable_key_t htableHash(const bor_list_t *key, void *_)
{
    const el_t *el = BOR_LIST_ENTRY(key, el_t, htable);
    return el->key;
}

int htableEq(const bor_list_t *key1, const bor_list_t *key2, void *_args)
{
    const pddl_action_args_t *args = args;
    const el_t *el1 = BOR_LIST_ENTRY(key1, el_t, htable);
    const el_t *el2 = BOR_LIST_ENTRY(key2, el_t, htable);
    return memcmp(el1->args, el2->args,
                  sizeof(pddl_obj_id_t) * args->arg_size) == 0;
}

void pddlActionArgsInit(pddl_action_args_t *args, int arg_size)
{
    size_t size = sizeof(el_t) + sizeof(pddl_obj_id_t) * arg_size;
    el_t *el = alloca(size);

    el->key = 0;
    borListInit(&el->htable);
    for (int i = 0; i < arg_size; ++i)
        el->args[i] = PDDL_OBJ_ID_UNDEF;

    bzero(args, sizeof(*args));
    args->arg_size = arg_size;
    args->arg_pool = borExtArrNew(sizeof(pddl_obj_id_t) * arg_size, NULL, el);
    args->htable = borHTableNew(htableHash, htableEq, args);
    args->args_size = 0;
}

void pddlActionArgsFree(pddl_action_args_t *args)
{
    borHTableDel(args->htable);
    borExtArrDel(args->arg_pool);
}

int pddlActionArgsAdd(pddl_action_args_t *args, const pddl_obj_id_t *a)
{
    el_t *el = borExtArrGet(args->arg_pool, args->args_size);
    el->id = args->arg_size;
    el->key = hash(a, args->arg_size);
    memcpy(el->args, a, sizeof(pddl_obj_id_t) * args->arg_size);
    borListInit(&el->htable);

    bor_list_t *ins = borHTableInsertUnique(args->htable, &el->htable);
    if (ins == NULL){
        ++args->args_size;
        return el->id;
    }else{
        el = BOR_LIST_ENTRY(ins, el_t, htable);
        return el->id;
    }
}

const pddl_obj_id_t *pddlActionArgsGet(const pddl_action_args_t *args, int id)
{
    if (id >= args->args_size)
        return NULL;
    const el_t *el = borExtArrGet(args->arg_pool, id);
    return el->args;
}

int pddlActionArgsSize(const pddl_action_args_t *args)
{
    return args->args_size;
}
