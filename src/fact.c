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

#include <boruvka/compiler.h>
#include <boruvka/alloc.h>
#include <boruvka/hfunc.h>
#include <boruvka/sort.h>
#include "pddl/pddl.h"
#include "pddl/fact.h"
#include "err.h"
#include "assert.h"

/** Deep copy of the fact.  */
static void pddlFactCopy(pddl_fact_t *dst, const pddl_fact_t *src);
/** Returns true if facts are equal.  */
static int pddlFactEq(const pddl_fact_t *f1, const pddl_fact_t *f2);

static bor_htable_key_t htableKey(const bor_list_t *key, void *_)
{
    pddl_fact_t *f = BOR_LIST_ENTRY(key, pddl_fact_t, htable);
    return f->hash;
}

static int htableEq(const bor_list_t *k1,
                    const bor_list_t *k2, void *_)
{
    pddl_fact_t *f1 = BOR_LIST_ENTRY(k1, pddl_fact_t, htable);
    pddl_fact_t *f2 = BOR_LIST_ENTRY(k2, pddl_fact_t, htable);
    return pddlFactEq(f1, f2);
}

static bor_htable_key_t pddlFactHash(const pddl_fact_t *f)
{
    return borHashSDBM(f->name);
}

static char *makeName(const pddl_ground_atom_t *ga, const pddl_t *pddl)
{
    char name[PDDL_FACT_MAX_NAME_SIZE];
    int offset;

    offset = snprintf(name, PDDL_FACT_MAX_NAME_SIZE, "%s",
                      pddl->pred.pred[ga->pred].name);
    for (int i = 0; i < ga->arg_size; ++i){
        offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                           " %s", pddl->obj.obj[ga->arg[i]].name);
    }
    name[PDDL_FACT_MAX_NAME_SIZE - 1] = 0x0;
    return BOR_STRDUP(name);
}

static pddl_fact_t *factFromGroundAtom(const pddl_ground_atom_t *ga,
                                       const pddl_t *pddl)
{
    pddl_fact_t *f;

    f = pddlFactNew();
    f->name = makeName(ga, pddl);
    f->ground_atom = pddlGroundAtomClone(ga);
    f->hash = pddlFactHash(f);
    return f;
}

void pddlFactInit(pddl_fact_t *f)
{
    bzero(f, sizeof(*f));
}

pddl_fact_t *pddlFactNew(void)
{
    pddl_fact_t *f = BOR_ALLOC(pddl_fact_t);
    pddlFactInit(f);
    return f;
}

void pddlFactFree(pddl_fact_t *f)
{
    if (f->name != NULL)
        BOR_FREE(f->name);
    if (f->ground_atom != NULL)
        pddlGroundAtomDel(f->ground_atom);
}

void pddlFactDel(pddl_fact_t *f)
{
    pddlFactFree(f);
    BOR_FREE(f);
}

static void pddlFactCopy(pddl_fact_t *dst, const pddl_fact_t *src)
{
    pddlFactFree(dst);
    if (src->name != NULL)
        dst->name = BOR_STRDUP(src->name);
    if (src->ground_atom != NULL)
        dst->ground_atom = pddlGroundAtomClone(src->ground_atom);
    dst->hash = pddlFactHash(dst);
}

int pddlFactCmp(const pddl_fact_t *f1, const pddl_fact_t *f2)
{
    return strcmp(f1->name, f2->name);
}

static int pddlFactEq(const pddl_fact_t *f1, const pddl_fact_t *f2)
{
    return pddlFactCmp(f1, f2) == 0;
}

static void makeSpace(pddl_facts_t *fs)
{
    if (fs->fact_size >= fs->fact_alloc){
        if (fs->fact_alloc == 0){
            fs->fact_alloc = 2;
        }else{
            fs->fact_alloc *= 2;
        }
        fs->fact = BOR_REALLOC_ARR(fs->fact, pddl_fact_t *, fs->fact_alloc);
    }
}

void pddlFactsInit(pddl_facts_t *fs)
{
    bzero(fs, sizeof(*fs));
    fs->htable = borHTableNew(htableKey, htableEq, fs);
}

void pddlFactsFree(pddl_facts_t *fs)
{
    pddl_fact_t *fact;

    borHTableDel(fs->htable);
    PDDL_FACTS_FOR_EACH(fs, fact)
        pddlFactDel(fact);
    if (fs->fact != NULL)
        BOR_FREE(fs->fact);
}

static int addFact(pddl_facts_t *fs, pddl_fact_t *fact)
{
    makeSpace(fs);
    fact->id = fs->fact_size;
    fs->fact[fs->fact_size] = fact;
    ++fs->fact_size;
    borHTableInsert(fs->htable, &fact->htable);
    return fact->id;
}

int pddlFactsAdd(pddl_facts_t *fs, const pddl_fact_t *f)
{
    bor_list_t *hfound;
    pddl_fact_t *fact;

    if ((hfound = borHTableFind(fs->htable, &f->htable)) != NULL)
        return (BOR_LIST_ENTRY(hfound, pddl_fact_t, htable))->id;

    fact = pddlFactNew();
    pddlFactCopy(fact, f);
    return addFact(fs, fact);
}

int pddlFactsAddGroundAtom(pddl_facts_t *fs, const pddl_ground_atom_t *ga,
                           const pddl_t *pddl)
{
    bor_list_t *hfound;
    pddl_fact_t *fact;

    fact = factFromGroundAtom(ga, pddl);
    if ((hfound = borHTableFind(fs->htable, &fact->htable)) != NULL){
        pddlFactDel(fact);
        fact = BOR_LIST_ENTRY(hfound, pddl_fact_t, htable);
        return fact->id;
    }

    return addFact(fs, fact);
}

void pddlFactsDelFact(pddl_facts_t *fs, int fact_id)
{
    pddl_fact_t *f;

    if (fs->fact[fact_id] == NULL)
        return;
    f = fs->fact[fact_id];
    borHTableErase(fs->htable, &f->htable);
    pddlFactDel(f);
    fs->fact[fact_id] = NULL;
}

void pddlFactsDelIrrelevantFacts(pddl_facts_t *fs,
                                 const int *irrelevant,
                                 int *remap)
{
    int size = 0;

    for (int fact_id = 0; fact_id < fs->fact_size; ++fact_id){
        if (irrelevant[fact_id]){
            remap[fact_id] = -1;
            pddlFactsDelFact(fs, fact_id);
        }else{
            remap[fact_id] = size;
            fs->fact[fact_id]->id = size;
            fs->fact[size++] = fs->fact[fact_id];
        }
    }

    fs->fact_size = size;
}

void pddlFactsCopy(pddl_facts_t *dst, const pddl_facts_t *src)
{
    for (int i = 0; i < src->fact_size; ++i)
        pddlFactsAdd(dst, src->fact[i]);
}

static int factCmpByName(const void *a, const void *b, void *_)
{
    const pddl_fact_t *f1 = *(pddl_fact_t **)a;
    const pddl_fact_t *f2 = *(pddl_fact_t **)b;
    return strcmp(f1->name, f2->name);
}

void pddlFactsSort(pddl_facts_t *fs, int *remap)
{
    borSort(fs->fact, fs->fact_size, sizeof(pddl_fact_t *),
            factCmpByName, NULL);
    for (int i = 0; i < fs->fact_size; ++i){
        pddl_fact_t *f = fs->fact[i];
        if (remap != NULL)
            remap[f->id] = i;
        f->id = i;
    }
}

static int printCmp(const void *_a, const void *_b, void *_fs)
{
    int a = *(int *)_a;
    int b = *(int *)_b;
    const pddl_facts_t *fs = _fs;
    return pddlFactCmp(fs->fact[a], fs->fact[b]);
}

void pddlFactsPrintSorted(const pddl_facts_t *fs,
                          const char *prefix, const char *suffix,
                          FILE *fout)
{
    int *sorted;

    sorted = BOR_ALLOC_ARR(int, fs->fact_size);
    for (int i = 0; i < fs->fact_size; ++i)
        sorted[i] = i;
    borSort(sorted, fs->fact_size, sizeof(int), printCmp, (void *)fs);
    for (int i = 0; i < fs->fact_size; ++i)
        fprintf(fout, "%s%s%s", prefix, fs->fact[sorted[i]]->name, suffix);
    BOR_FREE(sorted);
}

void pddlFactsIdSetPrintSorted(const bor_iset_t *set,
                               const pddl_facts_t *fs,
                               const char *prefix, const char *suffix,
                               FILE *fout)
{
    int *sorted;
    int size = borISetSize(set);

    sorted = BOR_ALLOC_ARR(int, size);
    for (int i = 0; i < size; ++i)
        sorted[i] = borISetGet(set, i);
    borSort(sorted, size, sizeof(int), printCmp, (void *)fs);
    for (int i = 0; i < size; ++i)
        fprintf(fout, "%s%s%s", prefix, fs->fact[sorted[i]]->name, suffix);
    BOR_FREE(sorted);
}
