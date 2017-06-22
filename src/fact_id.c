/***
 * cpddl
 * -------
 * Copyright (c)2017 Daniel Fiser <danfis@danfis.cz>,
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

int pddlFactIdSetHasId(const pddl_fact_id_set_t *s, int fact_id)
{
    // TODO: binary search
    int i;
    PDDL_FACT_ID_SET_FOR_EACH(s, i){
        if (i == fact_id)
            return 1;
    }
    return 0;
}

void pddlFactIdSetAdd(pddl_fact_id_set_t *s, int fact_id)
{
    if (s->size >= s->alloc){
        if (s->alloc == 0)
            s->alloc = 1;
        s->alloc *= 2;
        s->fact = BOR_REALLOC_ARR(s->fact, int, s->alloc);
    }
    s->fact[s->size++] = fact_id;

    if (s->size > 1 && fact_id < s->fact[s->size - 2]){
        int *f = s->fact + s->size - 1;
        for (; f > s->fact && f[0] < f[-1]; --f){
            int tmp = f[0];
            f[0] = f[-1];
            f[-1] = tmp;
        }
        if (f > s->fact && f[0] == f[-1]){
            for (--s->size; f != s->fact + s->size; ++f)
                *f = f[1];
        }
    }
}

void pddlFactIdSetCopy(pddl_fact_id_set_t *dst, const pddl_fact_id_set_t *src)
{
    for (int i = 0; i < src->size; ++i)
        pddlFactIdSetAdd(dst, src->fact[i]);
}

void pddlFactIdSetMinus(pddl_fact_id_set_t *s1, const pddl_fact_id_set_t *s2)
{
    int w, i, j;

    for (w = i = j = 0; i < s1->size && j < s2->size;){
        if (s1->fact[i] == s2->fact[j]){
            ++i;
            ++j;
        }else if (s1->fact[i] < s2->fact[j]){
            s1->fact[w++] = s1->fact[i++];
        }else{
            ++j;
        }
    }
    for (; i < s1->size; ++i, ++w)
        s1->fact[w] = s1->fact[i];
    s1->size = w + s1->size - i;
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
                              const pddl_fact_id_set_t *s, FILE *fout)
{
    int sorted[s->size];
    memcpy(sorted, s->fact, sizeof(int) * s->size);
    borSort(sorted, s->size, sizeof(int), factArrCmp, (void *)fs);

    for (int i = 0; i < s->size; ++i){
        if (i > 0)
            fprintf(fout, ", ");
        pddlFactPrint(pddl, fs->fact[sorted[i]], fout);
    }
    fprintf(fout, "\n");
}

