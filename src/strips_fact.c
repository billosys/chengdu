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

#include <boruvka/hfunc.h>
#include "pddl/strips_fact.h"

struct htable_fact {
    int id;
    const char *name;
    uint64_t hash;
    bor_list_t htable;
};
typedef struct htable_fact htable_fact_t;

static bor_htable_key_t htableKey(const bor_list_t *key, void *_fs)
{
    htable_fact_t *hf;
    hf = BOR_LIST_ENTRY(key, htable_fact_t, htable);
    return hf->hash;
}

static int htableEq(const bor_list_t *k1,
                    const bor_list_t *k2, void *_fs)
{
    htable_fact_t *hf1 = BOR_LIST_ENTRY(k1, htable_fact_t, htable);
    htable_fact_t *hf2 = BOR_LIST_ENTRY(k2, htable_fact_t, htable);
    if (hf1->hash != hf2->hash)
        return strcmp(hf1->name, hf2->name) == 0;
    return 0;
}

_bor_inline uint64_t nameHash(const char *name)
{
    return borHashDJB2(name);
}

void pddlStripsFactFree(pddl_strips_fact_t *f)
{
    if (f->name)
        BOR_FREE((char *)f->name);
}

void pddlStripsFactsInit(pddl_strips_facts_t *fs)
{
    bzero(fs, sizeof(*fs));
    fs->fact_alloc = 4;
    fs->fact = BOR_ALLOC_ARR(pddl_strips_fact_t, fs->fact_alloc);
    fs->htable = borHTableNew(htableKey, htableEq, fs);
}

void pddlStripsFactsFree(pddl_strips_facts_t *fs)
{
    bor_list_t hfs, *h, *tmp;
    int i;

    borListInit(&hfs);
    borHTableGather(fs->htable, &hfs);
    BOR_LIST_FOR_EACH_SAFE(&hfs, h, tmp)
        BOR_FREE(BOR_LIST_ENTRY(h, htable_fact_t, htable));
    borHTableDel(fs->htable);

    for (i = 0; i < fs->fact_size; ++i){
        pddlStripsFactFree(fs->fact + i);
    }
    BOR_FREE(fs->fact);
}

int pddlStripsFactsAddFromPDDLFact(pddl_strips_facts_t *fs,
                                   const pddl_t *pddl,
                                   const pddl_fact_t *fact)
{
    pddl_strips_fact_t *f;
    htable_fact_t *hf;
    int fid;
    char name[256];

    if (pddlFactFormat(&pddl->pred, &pddl->obj, fact, name, 256) != 0){
        fprintf(stderr, "Fatal Error:"
                        " Could not fit name of the fact into 256"
                        " characters.\n");
        exit(-1);
    }

    if ((fid = pddlStripsFactsFind(fs, name)) != -1)
        return fid;

    if (fs->fact_size >= fs->fact_alloc){
        fs->fact_alloc *= 2;
        fs->fact = BOR_REALLOC_ARR(fs->fact, pddl_strips_fact_t,
                                   fs->fact_alloc);
    }

    f = fs->fact + fs->fact_size++;
    f->name = BOR_STRDUP(name);
    f->neg = fact->neg;
    f->stat = fact->stat;
    f->is_private = fact->is_private;
    f->owner = fact->owner;

    hf = BOR_ALLOC(htable_fact_t);
    hf->id = fs->fact_size - 1;
    hf->name = f->name;
    hf->hash = nameHash(name);
    borListInit(&hf->htable);
    borHTableInsert(fs->htable, &hf->htable);

    return fs->fact_size - 1;
}

int pddlStripsFactsFind(pddl_strips_facts_t *fs, const char *name)
{
    htable_fact_t hf, *h;
    bor_list_t *k;

    hf.name = name;
    hf.hash = nameHash(name);

    if ((k = borHTableFind(fs->htable, &hf.htable)) != NULL){
        h = BOR_LIST_ENTRY(k, htable_fact_t, htable);
        return h->id;
    }
    return -1;
}
