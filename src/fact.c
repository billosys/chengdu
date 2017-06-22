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

void pddlFactCopy(pddl_fact_t *dst, const pddl_fact_t *src)
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

int pddlFactEq(const pddl_fact_t *f1, const pddl_fact_t *f2)
{
    return f1->pred == f2->pred
            && f1->arg_size == f2->arg_size
            && memcmp(f1->arg, f2->arg, sizeof(int) * f1->arg_size) == 0;
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

void pddlFactPrint(const pddl_t *pddl, const pddl_fact_t *f, FILE *fout)
{
    if (pddlFactIsStatic(pddl, f))
        fprintf(fout, "S:");
    if (f->is_private){
        fprintf(fout, "P");
        if (f->owner >= 0)
            fprintf(fout, "[%d]", f->owner);
        fprintf(fout, ":");
    }
    fprintf(fout, "%s:", pddl->pred.pred[f->pred].name);
    for (int i = 0; i < f->arg_size; ++i){
        fprintf(fout, " %s", pddl->obj.obj[f->arg[i]].name);
    }
}

void pddlFuncPrint(const pddl_t *pddl, const pddl_fact_t *f, FILE *fout)
{
    if (f->is_private){
        fprintf(fout, "P");
        if (f->owner >= 0)
            fprintf(fout, "[%d]", f->owner);
        fprintf(fout, ":");
    }
    fprintf(fout, "%s:", pddl->func.pred[f->pred].name);
    for (int i = 0; i < f->arg_size; ++i){
        fprintf(fout, " %s", pddl->obj.obj[f->arg[i]].name);
    }
    fprintf(fout, " --> %d", f->func_val);
}

int pddlFactIsStatic(const pddl_t *pddl, const pddl_fact_t *f)
{
    const pddl_pred_t *pred = pddl->pred.pred + f->pred;
    if (pddlPredIsStatic(pred))
        return 1;
    // TODO
    return 0;
}


static int factSetPrivate(const pddl_t *pddl, pddl_fact_t *fact)
{
    int ret;

    ret = pddlFactSetPrivate(pddl, fact);
    if (ret < 0){
        fprintf(stderr, "Error PDDL: Invalid definition of fact ");
        pddlFactPrint(pddl, fact, stderr);
        fprintf(stderr, ".\n");
        ERR2("The fact is defined so it should be private for two"
             " different agents.");
        return -1;
    }

    return 0;
}

static int parseObjsIntoArr(const pddl_lisp_node_t *n,
                            const pddl_objs_t *objs,
                            int from, int to, int **out, int *out_size)
{
    const pddl_lisp_node_t *c;
    int size, *obj, i;

    *out_size = size = to - from;
    *out = obj = BOR_CALLOC_ARR(int, size);
    for (i = 0; i < size; ++i){
        c = n->child + i + from;
        if (c->value == NULL){
            ERRN2(c, "Expecting object, got something else.");
            return -1;
        }

        obj[i] = pddlObjsGet(objs, c->value);
        if (obj[i] < 0){
            ERRN(c, "Unknown object `%s'.", c->value);
            return -1;
        }
    }

    return 0;
}

static int parseFunc(const pddl_lisp_node_t *n,
                     const pddl_t *pddl,
                     pddl_facts_t *fs)
{
    const pddl_lisp_node_t *nfunc, *nval;
    pddl_fact_t func;

    nfunc = n->child + 1;
    nval = n->child + 2;

    if (nfunc->child_size < 1
            || nfunc->child[0].value == NULL
            || nval->value == NULL){
        ERRN2(n, "Invalid function assignement.");
        return -1;
    }

    pddlFactInit(&func);
    func.func_val = atoi(nval->value);
    func.pred = pddlPredsGet(&pddl->func, nfunc->child[0].value);
    if (func.pred < 0){
        ERRN(nfunc, "Unknown function `%s'", nfunc->child[0].value);
        pddlFactFree(&func);
        return -1;
    }

    if (parseObjsIntoArr(nfunc, &pddl->obj, 1, nfunc->child_size,
                         &func.arg, &func.arg_size) != 0){
        pddlFactFree(&func);
        return -1;
    }

    if (factSetPrivate(pddl, &func) != 0){
        pddlFactFree(&func);
        return -1;
    }

    pddlFactsAdd(fs, &func);
    pddlFactFree(&func);
    return 0;
}

static int parseFact(const pddl_lisp_node_t *n,
                     const pddl_t *pddl,
                     const char *head,
                     pddl_facts_t *fs)
{
    pddl_fact_t fact;

    pddlFactInit(&fact);
    fact.pred = pddlPredsGet(&pddl->pred, head);
    if (fact.pred < 0){
        ERRN(n, "Unkwnown predicate `%s'.", head);
        pddlFactFree(&fact);
        return -1;
    }

    if (parseObjsIntoArr(n, &pddl->obj, 1, n->child_size,
                         &fact.arg, &fact.arg_size) != 0){
        pddlFactFree(&fact);
        return -1;
    }

    if (factSetPrivate(pddl, &fact) != 0){
        pddlFactFree(&fact);
        return -1;
    }

    pddlFactsAdd(fs, &fact);
    pddlFactFree(&fact);
    return 0;
}

static int parseFactFunc(pddl_t *pddl, const pddl_lisp_node_t *n)
{
    const char *head;

    if (n->child_size < 1){
        ERRN2(n, "Invalid fact in :init.");
        return -1;
    }

    head = pddlLispNodeHead(n);
    if (strcmp(head, "=") == 0
            && n->child_size == 3
            && n->child[1].value == NULL){
        return parseFunc(n, pddl, &pddl->init_func);
    }else{
        return parseFact(n, pddl, head, &pddl->init_fact);
    }
}

int pddlFactsParseInit(pddl_t *pddl)
{
    const pddl_lisp_node_t *ninit, *n;
    int i;

    ninit = pddlLispFindNode(&pddl->problem_lisp->root, PDDL_KW_INIT);
    if (ninit == NULL){
        ERR2("Missing :init.");
        return -1;
    }

    for (i = 1; i < ninit->child_size; ++i){
        n = ninit->child + i;
        if (parseFactFunc(pddl, n) != 0)
            return -1;
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

void pddlFactsSqueeze(pddl_facts_t *fs)
{
    fs->fact_alloc = fs->fact_size;
    fs->fact = BOR_REALLOC_ARR(fs->fact, pddl_fact_t *, fs->fact_alloc);
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

static int factCmp(const void *a, const void *b, void *_fs)
{
    const pddl_facts_t *fs = _fs;
    int fid1 = *(int *)a;
    int fid2 = *(int *)b;
    const pddl_fact_t *f1 = fs->fact[fid1];
    const pddl_fact_t *f2 = fs->fact[fid2];
    return pddlFactCmp(f1, f2);
}

static void _pddlFactsPrint(const pddl_t *pddl, const pddl_facts_t *fs,
                            int func, FILE *fout)
{
    const pddl_fact_t *f;

    PDDL_FACTS_FOR_EACH(fs, f){
        fprintf(fout, "% 4d: ", f->id);
        if (func){
            pddlFuncPrint(pddl, f, fout);
        }else{
            pddlFactPrint(pddl, f, fout);
        }
        fprintf(fout, "\n");
    }
}

static void _pddlFactsPrintSorted(const pddl_t *pddl,
                                  const pddl_facts_t *fs,
                                  int func, FILE *fout)
{
    int *sorted;

    sorted = BOR_ALLOC_ARR(int, fs->fact_size);
    for (int i = 0; i < fs->fact_size; ++i)
        sorted[i] = i;
    borSort(sorted, fs->fact_size, sizeof(int), factCmp, (void *)fs);
    for (int i = 0; i < fs->fact_size; ++i){
        fprintf(fout, "  ");
        if (func){
            pddlFuncPrint(pddl, fs->fact[sorted[i]], fout);
        }else{
            pddlFactPrint(pddl, fs->fact[sorted[i]], fout);
        }
        fprintf(fout, "\n");
    }
    BOR_FREE(sorted);
}

void pddlFactsPrint(const pddl_t *pddl, const pddl_facts_t *fs, FILE *fout)
{
    _pddlFactsPrint(pddl, fs, 0, fout);
}

void pddlFuncsPrint(const pddl_t *pddl, const pddl_facts_t *fs, FILE *fout)
{
    _pddlFactsPrint(pddl, fs, 1, fout);
}

void pddlFactsPrintSorted(const pddl_t *pddl, const pddl_facts_t *fs,
                          FILE *fout)
{
    _pddlFactsPrintSorted(pddl, fs, 0, fout);
}

void pddlFuncsPrintSorted(const pddl_t *pddl, const pddl_facts_t *fs,
                          FILE *fout)
{
    _pddlFactsPrintSorted(pddl, fs, 1, fout);
}

void pddlFactsPrintInit(const pddl_t *pddl, const pddl_facts_t *in, FILE *fout)
{
    fprintf(fout, "Init[%d]:\n", in->fact_size);
    pddlFactsPrint(pddl, in, fout);
}

void pddlFactsPrintInitFunc(const pddl_t *pddl,
                            const pddl_facts_t *in,
                            FILE *fout)
{
    fprintf(fout, "Init Func[%d]:\n", in->fact_size);
    pddlFuncsPrint(pddl, in, fout);
}
