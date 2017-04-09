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
#include "pddl/pddl.h"
#include "pddl/fact.h"
#include "err.h"

struct htable_fact {
    int id;
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
    pddl_facts_t *fs = _fs;
    htable_fact_t *hf1 = BOR_LIST_ENTRY(k1, htable_fact_t, htable);
    htable_fact_t *hf2 = BOR_LIST_ENTRY(k2, htable_fact_t, htable);
    pddl_fact_t *f1 = fs->fact + hf1->id;
    pddl_fact_t *f2 = fs->fact + hf2->id;
    return pddlFactCmp(f1, f2) == 0;
}

static uint64_t pddlFactHash(const pddl_fact_t *f)
{
    char *data;
    uint64_t hash;
    size_t size, offset;

    offset = bor_offsetof(pddl_fact_t, arg_size);
    size = sizeof(*f) + sizeof(int) * f->arg_size;
    data = BOR_ALLOC_ARR(char, size);
    memcpy(data, f, sizeof(*f));
    memcpy(data + sizeof(*f), f->arg, sizeof(int) * f->arg_size);
    hash = borCityHash_64(data + offset, size - offset);
    BOR_FREE(data);
    return hash;
}

void pddlFactInit(pddl_fact_t *f)
{
    bzero(f, sizeof(*f));
    f->owner = -1;
}

void pddlFactFromPred(pddl_fact_t *f, int pred_id, const pddl_pred_t *pred)
{
    pddlFactInit(f);
    f->pred = pred_id;
    f->arg_size = pred->param_size;
    f->arg = BOR_CALLOC_ARR(int, f->arg_size);
    f->stat = !pred->write;
}

void pddlFactFree(pddl_fact_t *f)
{
    if (f->arg != NULL)
        BOR_FREE(f->arg);
}

void pddlFactCopy(pddl_fact_t *dst, const pddl_fact_t *src)
{
    *dst = *src;
    if (src->arg != NULL){
        dst->arg = BOR_ALLOC_ARR(int, src->arg_size);
        memcpy(dst->arg, src->arg, sizeof(int) * src->arg_size);
    }
}

int pddlFactCmp(const pddl_fact_t *f1, const pddl_fact_t *f2)
{
    int cmp;

    cmp = memcmp((void *)&f1->arg_size, (void *)&f2->arg_size,
                 sizeof(*f1) - bor_offsetof(pddl_fact_t, arg_size));
    if (cmp == 0)
        cmp = memcmp(f1->arg, f2->arg, sizeof(int) * f1->arg_size);
    return cmp;
}

int pddlFactSetPrivate(pddl_fact_t *fact,
                       const pddl_preds_t *pred,
                       const pddl_objs_t *objs)
{
    const pddl_pred_t *pr;
    const pddl_obj_t *obj;
    int i;

    pr = pred->pred + fact->pred;
    if (pr->is_private){
        fact->is_private = 1;
        if (pr->owner_param >= 0)
            fact->owner = fact->arg[pr->owner_param];
    }

    for (i = 0; i < fact->arg_size; ++i){
        obj = objs->obj + fact->arg[i];
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

#define STRNFMTCPY(str, size, fmt) \
    do { \
        int __w = snprintf((str), (size), (fmt)); \
        if ((size) - __w <= 0) \
            return -1; \
        (str) += __w; \
        (size) -= __w; \
    } while (0)

#define STRNFMT(str, size, fmt, ...) \
    do { \
        int __w = snprintf((str), (size), (fmt), __VA_ARGS__); \
        if ((size) - __w <= 0) \
            return -1; \
        (str) += __w; \
        (size) -= __w; \
    } while (0)

int pddlFactFormat(const pddl_preds_t *predicates,
                   const pddl_objs_t *objs,
                   const pddl_fact_t *f,
                   char *str,
                   int strsize)
{
    int size = strsize, i;

    if (f->neg)
        STRNFMTCPY(str, size, "N:");
    if (f->stat)
        STRNFMTCPY(str, size, "S:");
    if (f->is_private){
        STRNFMTCPY(str, size, "P");
        if (f->owner >= 0)
            STRNFMT(str, size, "[%d]", f->owner);
        STRNFMTCPY(str, size, ":");
    }
    STRNFMT(str, size, "%s:", predicates->pred[f->pred].name);
    for (i = 0; i < f->arg_size; ++i){
        STRNFMT(str, size, " %s", objs->obj[f->arg[i]].name);
    }
    return 0;
}

void pddlFactPrint(const pddl_preds_t *predicates,
                   const pddl_objs_t *objs,
                   const pddl_fact_t *f,
                   FILE *fout)
{
    char name[128];

    if (pddlFactFormat(predicates, objs, f, name, 128) != 0){
        fprintf(stderr, "Fatal Error:"
                        " Could not fit name of the fact into 128"
                        " characters.\n");
        exit(-1);
    }
    fprintf(fout, "%s", name);
}



static int factSetPrivate(pddl_fact_t *fact,
                          const pddl_preds_t *pred,
                          const pddl_objs_t *objs)
{
    int ret;

    ret = pddlFactSetPrivate(fact, pred, objs);
    if (ret < 0){
        fprintf(stderr, "Error PDDL: Invalid definition of fact ");
        pddlFactPrint(pred, objs, fact, stderr);
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
                     const pddl_preds_t *functions,
                     const pddl_objs_t *objs,
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
    func.pred = pddlPredsGet(functions, nfunc->child[0].value);
    if (func.pred < 0){
        ERRN(nfunc, "Unknown function `%s'", nfunc->child[0].value);
        pddlFactFree(&func);
        return -1;
    }

    if (parseObjsIntoArr(nfunc, objs, 1, nfunc->child_size,
                         &func.arg, &func.arg_size) != 0){
        pddlFactFree(&func);
        return -1;
    }

    if (factSetPrivate(&func, functions, objs) != 0){
        pddlFactFree(&func);
        return -1;
    }

    pddlFactsAdd(fs, &func);
    pddlFactFree(&func);
    return 0;
}

static int parseFact(const pddl_lisp_node_t *n,
                     const pddl_preds_t *predicates,
                     const pddl_objs_t *objs,
                     const char *head, pddl_facts_t *fs)
{
    pddl_fact_t fact;

    pddlFactInit(&fact);
    fact.pred = pddlPredsGet(predicates, head);
    if (fact.pred < 0){
        ERRN(n, "Unkwnown predicate `%s'.", head);
        pddlFactFree(&fact);
        return -1;
    }

    if (parseObjsIntoArr(n, objs, 1, n->child_size,
                         &fact.arg, &fact.arg_size) != 0){
        pddlFactFree(&fact);
        return -1;
    }

    if (factSetPrivate(&fact, predicates, objs) != 0){
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
        return parseFunc(n, &pddl->func, &pddl->obj, &pddl->init_func);
    }else{
        return parseFact(n, &pddl->pred, &pddl->obj, head, &pddl->init_fact);
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

static void pddlFactsReserve1(pddl_facts_t *fs)
{
    if (fs->fact_size >= fs->fact_alloc){
        if (fs->fact_alloc == 0){
            fs->fact_alloc = 2;
        }else{
            fs->fact_alloc *= 2;
        }
        fs->fact = BOR_REALLOC_ARR(fs->fact, pddl_fact_t, fs->fact_alloc);
    }
}

void pddlFactsInit(pddl_facts_t *fs)
{
    bzero(fs, sizeof(*fs));
    fs->htable = borHTableNew(htableKey, htableEq, fs);
}

void pddlFactsFree(pddl_facts_t *fs)
{
    bor_list_t flist, *h, *tmp;
    int i;

    borListInit(&flist);
    borHTableGather(fs->htable, &flist);
    BOR_LIST_FOR_EACH_SAFE(&flist, h, tmp){
        BOR_FREE(BOR_LIST_ENTRY(h, htable_fact_t, htable));
    }
    borHTableDel(fs->htable);

    for (i = 0; i < fs->fact_size; ++i)
        pddlFactFree(fs->fact + i);
    if (fs->fact != NULL)
        BOR_FREE(fs->fact);
}

int pddlFactsAdd(pddl_facts_t *fs, const pddl_fact_t *sf)
{
    pddl_fact_t *f;
    htable_fact_t *hf;
    int id;

    if ((id = pddlFactsFind(fs, sf)) >= 0)
        return id;

    pddlFactsReserve1(fs);
    f = fs->fact + fs->fact_size++;
    pddlFactCopy(f, sf);

    hf = BOR_ALLOC(htable_fact_t);
    hf->id = fs->fact_size - 1;
    hf->hash = pddlFactHash(f);
    borListInit(&hf->htable);
    borHTableInsert(fs->htable, &hf->htable);

    return fs->fact_size - 1;
}

void pddlFactsSqueeze(pddl_facts_t *fs)
{
    fs->fact_alloc = fs->fact_size;
    fs->fact = BOR_REALLOC_ARR(fs->fact, pddl_fact_t, fs->fact_alloc);
}

void pddlFactsReserve(pddl_facts_t *fs, int alloc)
{
    if (fs->fact_alloc >= alloc)
        return;
    fs->fact_alloc = alloc;
    fs->fact = BOR_REALLOC_ARR(fs->fact, pddl_fact_t, fs->fact_alloc);
}

void pddlFactsCopy(pddl_facts_t *dst, const pddl_facts_t *src)
{
    int i;

    pddlFactsInit(dst);
    for (i = 0; i < src->fact_size; ++i)
        pddlFactsAdd(dst, src->fact + i);
}

int pddlFactsFind(pddl_facts_t *fs, const pddl_fact_t *f)
{
    htable_fact_t hf, *h;
    bor_list_t *k;

    if (fs->fact_size == 0)
        return -1;

    pddlFactsReserve1(fs);
    fs->fact[fs->fact_size] = *f;

    hf.id = fs->fact_size;
    hf.hash = pddlFactHash(f);
    if ((k = borHTableFind(fs->htable, &hf.htable)) == NULL)
        return -1;
    h = BOR_LIST_ENTRY(k, htable_fact_t, htable);
    return h->id;
}

static void printFact(const pddl_preds_t *predicates,
                      const pddl_objs_t *objs,
                      const pddl_fact_t *f,
                      int func_val, FILE *fout)
{
    fprintf(fout, "    ");
    pddlFactPrint(predicates, objs, f, fout);
    if (func_val != 0)
        fprintf(fout, " --> %d", f->func_val);
    fprintf(fout, "\n");
}

static void pddlFactsPrint(const pddl_preds_t *predicates,
                           const pddl_objs_t *objs,
                           const pddl_facts_t *in,
                           const char *header,
                           int func_val,
                           FILE *fout)
{
    int i;

    fprintf(fout, "%s[%d]:\n", header, in->fact_size);
    for (i = 0; i < in->fact_size; ++i)
        printFact(predicates, objs, in->fact + i, func_val, fout);
}

void pddlFactsPrintInit(const pddl_preds_t *predicates,
                        const pddl_objs_t *objs,
                        const pddl_facts_t *in,
                        FILE *fout)
{
    pddlFactsPrint(predicates, objs, in, "Init", 0, fout);
}

void pddlFactsPrintInitFunc(const pddl_preds_t *predicates,
                            const pddl_objs_t *objs,
                            const pddl_facts_t *in,
                            FILE *fout)
{
    pddlFactsPrint(predicates, objs, in, "Init Func", 1, fout);
}

void pddlFactsPrintGoal(const pddl_preds_t *predicates,
                        const pddl_objs_t *objs,
                        const pddl_facts_t *in,
                        FILE *fout)
{
    pddlFactsPrint(predicates, objs, in, "Goal", 0, fout);
}
