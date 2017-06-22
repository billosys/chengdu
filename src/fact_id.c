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
#include "pddl/fact_id.h"
#include "err.h"
#include "assert.h"

void pddlFactIdSetAdd(pddl_fact_id_set_t *arr, int fact_id)
{
    if (arr->size >= arr->alloc){
        if (arr->alloc == 0)
            arr->alloc = 1;
        arr->alloc *= 2;
        arr->fact = BOR_REALLOC_ARR(arr->fact, int, arr->alloc);
    }
    arr->fact[arr->size++] = fact_id;

    if (arr->size > 1 && fact_id < arr->fact[arr->size - 2]){
        int *f = arr->fact + arr->size - 1;
        for (; f > arr->fact && f[0] < f[-1]; --f){
            int tmp = f[0];
            f[0] = f[-1];
            f[-1] = tmp;
        }
        if (f > arr->fact && f[0] == f[-1]){
            for (--arr->size; f != arr->fact + arr->size; ++f)
                *f = f[1];
        }
    }
}

void pddlFactIdSetCopy(pddl_fact_id_set_t *dst, const pddl_fact_id_set_t *src)
{
    dst->alloc = dst->size = src->size;
    dst->fact = BOR_ALLOC_ARR(int, dst->alloc);
    memcpy(dst->fact, src->fact, sizeof(int) * src->size);
}

void pddlFactIdSetMinus(pddl_fact_id_set_t *a1, const pddl_fact_id_set_t *a2)
{
    int w, i, j;

    for (w = i = j = 0; i < a1->size && j < a2->size;){
        if (a1->fact[i] == a2->fact[j]){
            ++i;
            ++j;
        }else if (a1->fact[i] < a2->fact[j]){
            a1->fact[w++] = a1->fact[i++];
        }else{
            ++j;
        }
    }
    for (; i < a1->size; ++i, ++w)
        a1->fact[w] = a1->fact[i];
    a1->size = w + a1->size - i;
}

static int factArrCmp(const void *a, const void *b, void *_fs)
{
    const pddl_facts_t *fs = _fs;
    int fid1 = *(int *)a;
    int fid2 = *(int *)b;
    const pddl_fact_t *f1 = fs->fact[fid1];
    const pddl_fact_t *f2 = fs->fact[fid2];
    return pddlFactCmp(f1, f2);
}


void pddlFactIdSetPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                              const pddl_fact_id_set_t *arr, FILE *fout)
{
    int sorted[arr->size];
    memcpy(sorted, arr->fact, sizeof(int) * arr->size);
    borSort(sorted, arr->size, sizeof(int), factArrCmp, (void *)fs);

    for (int i = 0; i < arr->size; ++i){
        if (i > 0)
            fprintf(fout, ", ");
        pddlFactPrint(pddl, fs->fact[sorted[i]], fout);
    }
    fprintf(fout, "\n");
}

