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

static bor_htable_key_t htableKey(const bor_list_t *key, void *_)
{
    pddl_strips_fact_t *f;
    f = BOR_LIST_ENTRY(key, pddl_strips_fact_t, htable);
    return f->name_hash;
}

static int htableEq(const bor_list_t *k1,
                    const bor_list_t *k2, void *_)
{
    pddl_strips_fact_t *f1 = BOR_LIST_ENTRY(k1, pddl_strips_fact_t, htable);
    pddl_strips_fact_t *f2 = BOR_LIST_ENTRY(k2, pddl_strips_fact_t, htable);
    if (f1->name_hash != f2->name_hash)
        return strcmp(f1->name, f2->name) == 0;
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
    int i;

    borHTableDel(fs->htable);
    for (i = 0; i < fs->fact_size; ++i){
        pddlStripsFactFree(fs->fact + i);
    }
    BOR_FREE(fs->fact);
}

void pddlStripsFactsAddFromPDDLFact(pddl_strips_facts_t *fs,
                                    const pddl_t *pddl,
                                    const pddl_fact_t *fact)
{
    pddl_strips_fact_t *f;
    char name[256];

    if (pddlFactFormat(&pddl->pred, &pddl->obj, fact, name, 256) != 0){
        fprintf(stderr, "Fatal Error:"
                        " Could not fit name of the fact into 256"
                        " characters.\n");
        exit(-1);
    }

    if (pddlStripsFactsFind(fs, name) != NULL)
        return;

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

    f->name_hash = nameHash(name);
    borListInit(&f->htable);
    borHTableInsert(fs->htable, &f->htable);
}

pddl_strips_fact_t *pddlStripsFactsFind(pddl_strips_facts_t *fs,
                                        const char *name)
{
    pddl_strips_fact_t f;
    bor_list_t *k;

    f.name = name;
    f.name_hash = nameHash(name);

    if ((k = borHTableFind(fs->htable, &f.htable)) != NULL)
        return BOR_LIST_ENTRY(k, pddl_strips_fact_t, htable);
    return NULL;
}
