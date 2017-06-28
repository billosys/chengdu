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

#include <boruvka/alloc.h>
#include <boruvka/sort.h>
#include "pddl/mgroup.h"

void pddlMGroupInit(pddl_mgroup_t *mg)
{
    bzero(mg, sizeof(*mg));
    borISetInit(&mg->fact);
}

void pddlMGroupFree(pddl_mgroup_t *mg)
{
    borISetFree(&mg->fact);
}

pddl_mgroups_t *pddlMGroupsNew(void)
{
    pddl_mgroups_t *mgs;
    mgs = BOR_ALLOC(pddl_mgroups_t);
    bzero(mgs, sizeof(*mgs));
    return mgs;
}

void pddlMGroupsDel(pddl_mgroups_t *mgs)
{
    for (int i = 0; i < mgs->size; ++i)
        pddlMGroupFree(mgs->g + i);
    BOR_FREE(mgs);
}

pddl_mgroup_t *pddlMGroupsAdd(pddl_mgroups_t *mgs, const bor_iset_t *mg)
{
    pddl_mgroup_t *g;

    if (mgs->size >= mgs->alloc){
        if (mgs->alloc == 0)
            mgs->alloc = 1;
        mgs->alloc *= 2;
        mgs->g = BOR_REALLOC_ARR(mgs->g, pddl_mgroup_t, mgs->alloc);
    }

    g = mgs->g + mgs->size++;
    pddlMGroupInit(g);
    borISetUnion(&g->fact, mg);
    return g;
}



static int prettyMutexCmp(const void *a, const void *b, void *_fs)
{
    const pddl_facts_t *fs = _fs;
    int *m1 = *(int **)a;
    int *m2 = *(int **)b;
    if (m1[0] != m2[0])
        return m1[0] - m2[0];
    for (int i = 0; i < m1[0]; ++i){
        const pddl_fact_t *f1 = fs->fact[m1[i + 1]];
        const pddl_fact_t *f2 = fs->fact[m2[i + 1]];
        int cmp = pddlFactCmp(f1, f2);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

static int prettyFactCmp(const void *a, const void *b, void *_fs)
{
    const pddl_facts_t *fs = _fs;
    int fid1 = *(int *)a;
    int fid2 = *(int *)b;
    const pddl_fact_t *f1 = fs->fact[fid1];
    const pddl_fact_t *f2 = fs->fact[fid2];
    return pddlFactCmp(f1, f2);
}

struct pretty {
    int **m;
    int size;
};
typedef struct pretty pretty_t;

void pddlMGroupsPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                            const pddl_mgroups_t *ms, FILE *fout)
{
    pretty_t p;

    if (ms->size == 0)
        return;

    p.size = ms->size;
    p.m = BOR_ALLOC_ARR(int *, ms->size);
    for (int i = 0; i < ms->size; ++i){
        p.m[i] = BOR_ALLOC_ARR(int, ms->g[i].fact.size + 1);
        p.m[i][0] = ms->g[i].fact.size;
        for (int j = 0; j < ms->g[i].fact.size; ++j)
            p.m[i][j + 1] = borISetGet(&ms->g[i].fact, j);
        borSort(p.m[i] + 1, ms->g[i].fact.size, sizeof(int),
                prettyFactCmp, (void *)fs);
    }
    borSort(p.m, p.size, sizeof(int *), prettyMutexCmp, (void *)fs);

    for (int i = 0; i < p.size; ++i){
        if (ms->g[i].is_init)
            fprintf(fout, "i:");
        if (ms->g[i].is_goal)
            fprintf(fout, "g:");
        if (ms->g[i].is_fa)
            fprintf(fout, "fa:");
        fprintf(fout, "%d :: ", p.m[i][0]);
        for (int j = 0; j < p.m[i][0]; ++j){
            if (j > 0)
                fprintf(fout, "; ");
            pddlFactPrint(pddl, fs->fact[p.m[i][j + 1]], fout);
        }
        fprintf(fout, "\n");
    }

    for (int i = 0; i < ms->size; ++i)
        BOR_FREE(p.m[i]);
    BOR_FREE(p.m);
}
