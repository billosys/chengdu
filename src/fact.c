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
/** Set the name of the fact */
static void pddlFactSetName(pddl_fact_t *dst, const pddl_t *pddl);
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

static uint64_t pddlFactHash(const pddl_fact_t *f)
{
    uint64_t hash;

    hash = borCityHash_32(&f->pred, sizeof(int));
    hash <<= 32u;
    hash |= 0xffffffffu & borCityHash_32(f->arg, sizeof(int) * f->arg_size);
    return hash;
}

void pddlFactInit(pddl_fact_t *f)
{
    bzero(f, sizeof(*f));
    f->owner = -1;
}

pddl_fact_t *pddlFactNew(void)
{
    pddl_fact_t *f = BOR_ALLOC(pddl_fact_t);
    pddlFactInit(f);
    return f;
}

void pddlFactFree(pddl_fact_t *f)
{
    if (f->arg != NULL)
        BOR_FREE(f->arg);
}

void pddlFactDel(pddl_fact_t *f)
{
    pddlFactFree(f);
    BOR_FREE(f);
}

static void pddlFactCopy(pddl_fact_t *dst, const pddl_fact_t *src)
{
    dst->pred = src->pred;
    dst->arg_size = src->arg_size;
    dst->func_val = src->func_val;
    dst->arg = NULL;
    if (src->arg != NULL){
        dst->arg = BOR_ALLOC_ARR(int, src->arg_size);
        memcpy(dst->arg, src->arg, sizeof(int) * src->arg_size);
    }
}

int pddlFactCmp(const pddl_fact_t *f1, const pddl_fact_t *f2)
{
    if (f1->pred != f2->pred)
        return f1->pred - f2->pred;
    ASSERT(f1->arg_size == f2->arg_size);
    return memcmp(f1->arg, f2->arg, sizeof(int) * f1->arg_size);
}

static int pddlFactEq(const pddl_fact_t *f1, const pddl_fact_t *f2)
{
    return pddlFactCmp(f1, f2) == 0;
}

int pddlFactSetPrivate(const pddl_t *pddl, pddl_fact_t *fact)
{
    const pddl_pred_t *pr;
    const pddl_obj_t *obj;
    int i;

    pr = pddl->pred.pred + fact->pred;
    if (pr->is_private){
        fact->is_private = 1;
        if (pr->owner_param >= 0)
            fact->owner = fact->arg[pr->owner_param];
    }

    for (i = 0; i < fact->arg_size; ++i){
        obj = pddl->obj.obj + fact->arg[i];
        if (obj->is_private){
            if (fact->is_private){
                if (obj->owner >= 0 && fact->owner != obj->owner){
                    return -1;
                }
            }else{
                fact->is_private = 1;
                if (obj->owner >= 0)
                    fact->owner = obj->owner;
            }
        }
    }

    return fact->is_private;
}

int pddlFactIsStatic(const pddl_t *pddl, const pddl_fact_t *f)
{
    const pddl_pred_t *pred = pddl->pred.pred + f->pred;
    if (pddlPredIsStatic(pred))
        return 1;
    // TODO
    return 0;
}

static char name[PDDL_FACT_MAX_NAME_SIZE];
static int factFuncName(const pddl_fact_t *f, const pddl_t *pddl,
                        const pddl_preds_t *pred, int offset)
{
    offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset, "%s",
                       pred->pred[f->pred].name);
    for (int i = 0; i < f->arg_size; ++i){
        offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                           " %s", pddl->obj.obj[f->arg[i]].name);
    }
    name[PDDL_FACT_MAX_NAME_SIZE - 1] = 0x0;
    return offset;
}
static int factName(const pddl_fact_t *f, const pddl_t *pddl, int offset)
{
    return factFuncName(f, pddl, &pddl->pred, offset);
}

const char *pddlFactName(const pddl_fact_t *f, const pddl_t *pddl)
{
    factName(f, pddl, 0);
    return name;
}

const char *pddlFactNamePDDL(const pddl_fact_t *f, const pddl_t *pddl)
{
    int offset;
    name[0] = '(';
    offset = factName(f, pddl, 1);
    snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset, ")");
    name[PDDL_FACT_MAX_NAME_SIZE - 1] = 0;
    return name;
}

const char *pddlFactNameDebug(const pddl_fact_t *f, const pddl_t *pddl)
{
    int offset = 0;

    if (pddlFactIsStatic(pddl, f)){
        offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                           "S:");
    }
    if (f->is_private){
        offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                           "P");
        if (f->owner >= 0){
            offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                              "[%d]", f->owner);
        }
        offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                           ":");
    }
    offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset, "(");
    offset = factName(f, pddl, offset);
    snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset, ")");
    name[PDDL_FACT_MAX_NAME_SIZE - 1] = 0x0;

    return name;
}

static int funcName(const pddl_fact_t *f, const pddl_t *pddl, int offset)
{
    return factFuncName(f, pddl, &pddl->func, offset);
}

const char *pddlFuncName(const pddl_fact_t *f, const pddl_t *pddl)
{
    funcName(f, pddl, 0);
    return name;
}

const char *pddlFuncNamePDDL(const pddl_fact_t *f, const pddl_t *pddl)
{
    int offset;
    name[0] = '(';
    offset = funcName(f, pddl, 1);
    snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset, ")");
    name[PDDL_FACT_MAX_NAME_SIZE - 1] = 0;
    return name;
}

const char *pddlFuncNameDebug(const pddl_fact_t *f, const pddl_t *pddl)
{
    int offset = 0;

    if (f->is_private){
        offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                           "P");
        if (f->owner >= 0){
            offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                              "[%d]", f->owner);
        }
        offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
                           ":");
    }
    offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset, "(");
    offset = funcName(f, pddl, offset);
    offset += snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset, ")");
    snprintf(name + offset, PDDL_FACT_MAX_NAME_SIZE - offset,
             " --> %d", f->func_val);
    name[PDDL_FACT_MAX_NAME_SIZE - 1] = 0x0;
    return name;
}




static int factSetPrivate(const pddl_t *pddl, pddl_fact_t *fact)
{
    int ret;

    ret = pddlFactSetPrivate(pddl, fact);
    if (ret < 0){
        ERR_RET2(-1., "The fact is defined so it is private for two"
                      " different agents.");
    }

    return 0;
}

static pddl_fact_t *nextNewFact(pddl_facts_t *fs)
{
    pddl_fact_t *f;

    if (fs->fact_size >= fs->fact_alloc){
        if (fs->fact_alloc == 0){
            fs->fact_alloc = 2;
        }else{
            fs->fact_alloc *= 2;
        }
        fs->fact = BOR_REALLOC_ARR(fs->fact, pddl_fact_t *, fs->fact_alloc);
    }

    f = pddlFactNew();
    f->id = fs->fact_size;
    fs->fact[fs->fact_size] = f;
    ++fs->fact_size;
    return f;
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

int pddlFactsAdd(pddl_facts_t *fs, const pddl_fact_t *sf)
{
    pddl_fact_t find, *f;
    bor_list_t *hfound;

    find = *sf;
    find.hash = pddlFactHash(sf);
    if ((hfound = borHTableFind(fs->htable, &find.htable)) != NULL){
        f = BOR_LIST_ENTRY(hfound, pddl_fact_t, htable);
        return f->id;
    }

    f = nextNewFact(fs);
    pddlFactCopy(f, sf);
    f->hash = find.hash;
    borHTableInsert(fs->htable, &f->htable);
    return f->id;
}

int pddlFactsAddGroundAtom(pddl_facts_t *fs, const pddl_ground_atom_t *ga,
                           const pddl_t *pddl)
{
    bor_list_t *hfound;
    pddl_fact_t *out;
    PDDL_FACT_STACK(locf, ga->arg_size);

    locf.pred = ga->pred;
    locf.arg_size = ga->arg_size;
    memcpy(locf.arg, ga->arg, sizeof(int) * ga->arg_size);
    locf.hash = pddlFactHash(&locf);

    if ((hfound = borHTableFind(fs->htable, &locf.htable)) != NULL){
        out = BOR_LIST_ENTRY(hfound, pddl_fact_t, htable);
        return out->id;
    }

    out = nextNewFact(fs);
    pddlFactCopy(out, &locf);
    out->hash = locf.hash;
    borHTableInsert(fs->htable, &out->htable);
    return out->id;
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
    int i;

    for (i = 0; i < src->fact_size; ++i)
        pddlFactsAdd(dst, src->fact[i]);
}

int pddlFactsFind(const pddl_facts_t *fs, const pddl_fact_t *ff)
{
    pddl_fact_t find, *f;
    bor_list_t *hfound;

    find = *ff;
    find.hash = pddlFactHash(ff);
    if ((hfound = borHTableFind(fs->htable, &find.htable)) != NULL){
        f = BOR_LIST_ENTRY(hfound, pddl_fact_t, htable);
        return f->id;
    }
    return -1;
}

void pddlFactsPrint(const pddl_facts_t *fs, const pddl_t *pddl,
                    const char *(*name)(const pddl_fact_t *, const pddl_t *),
                    const char *prefix, const char *suffix,
                    FILE *fout)
{
    for (int i = 0; i < fs->fact_size; ++i)
        fprintf(fout, "%s%s%s", prefix, name(fs->fact[i], pddl), suffix);
}


struct name_sort {
    const pddl_fact_t *fact;
    char *name;
};

static int factNameCmp(const void *_a, const void *_b, void *_)
{
    const struct name_sort *a = _a;
    const struct name_sort *b = _b;
    return strcmp(a->name, b->name);
}

void pddlFactsPrintSorted(const pddl_facts_t *fs, const pddl_t *pddl,
                    const char *(*name)(const pddl_fact_t *, const pddl_t *),
                    const char *prefix, const char *suffix,
                    FILE *fout)
{
    struct name_sort *sorted;

    sorted = BOR_ALLOC_ARR(struct name_sort, fs->fact_size);
    for (int i = 0; i < fs->fact_size; ++i){
        sorted[i].fact = fs->fact[i];
        sorted[i].name = BOR_STRDUP(pddlFactName(fs->fact[i], pddl));
    }
    borSort(sorted, fs->fact_size, sizeof(struct name_sort),
            factNameCmp, NULL);

    for (int i = 0; i < fs->fact_size; ++i)
        fprintf(fout, "%s%s%s", prefix, name(sorted[i].fact, pddl), suffix);

    for (int i = 0; i < fs->fact_size; ++i)
        BOR_FREE(sorted[i].name);
    BOR_FREE(sorted);
}

void pddlFactsIdSetPrintSorted(const bor_iset_t *set,
                    const pddl_facts_t *fs, const pddl_t *pddl,
                    const char *(*name)(const pddl_fact_t *, const pddl_t *),
                    const char *prefix, const char *suffix,
                    FILE *fout)
{
    struct name_sort *sorted;
    int size = borISetSize(set);

    sorted = BOR_ALLOC_ARR(struct name_sort, fs->fact_size);
    for (int i = 0; i < size; ++i){
        sorted[i].fact = fs->fact[borISetGet(set, i)];
        sorted[i].name = BOR_STRDUP(pddlFactName(sorted[i].fact, pddl));
    }
    borSort(sorted, size, sizeof(struct name_sort), factNameCmp, NULL);

    for (int i = 0; i < size; ++i)
        fprintf(fout, "%s%s%s", prefix, name(sorted[i].fact, pddl), suffix);

    for (int i = 0; i < size; ++i)
        BOR_FREE(sorted[i].name);
    BOR_FREE(sorted);
}
