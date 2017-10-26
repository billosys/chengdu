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

#include <boruvka/sort.h>
#include <pddl/mutex.h>

void pddlMutexInit(pddl_mutex_t *m)
{
    borISetInit(&m->fact);
}

void pddlMutexFree(pddl_mutex_t *m)
{
    borISetFree(&m->fact);
}

void pddlMutexesInit(pddl_mutexes_t *ms)
{
    bzero(ms, sizeof(*ms));
}

void pddlMutexesFree(pddl_mutexes_t *ms)
{
    pddl_mutex_t *m;
    PDDL_MUTEXES_FOR_EACH(ms, m)
        pddlMutexFree(m);
    if (ms->m != NULL)
        BOR_FREE(ms->m);
}

pddl_mutexes_t *pddlMutexesNew(void)
{
    pddl_mutexes_t *ms;

    ms = BOR_ALLOC(pddl_mutexes_t);
    pddlMutexesInit(ms);
    return ms;
}

void pddlMutexesDel(pddl_mutexes_t *ms)
{
    pddlMutexesFree(ms);
    BOR_FREE(ms);
}

int pddlMutexesIsMutex(const pddl_mutexes_t *ms, const bor_iset_t *facts)
{
    for (int i = 0; i < ms->size; ++i){
        const pddl_mutex_t *mutex = ms->m + i;
        if (borISetIsSubset(&mutex->fact, facts))
            return 1;
    }
    return 0;
}

pddl_mutex_t *pddlMutexesAdd(pddl_mutexes_t *ms, const bor_iset_t *m)
{
    if (ms->size >= ms->alloc){
        if (ms->alloc == 0)
            ms->alloc = 1;
        ms->alloc *= 2;
        ms->m = BOR_REALLOC_ARR(ms->m, pddl_mutex_t, ms->alloc);
    }

    pddlMutexInit(ms->m + ms->size);
    borISetUnion(&ms->m[ms->size].fact, m);
    ++ms->size;

    return ms->m + ms->size - 1;
}

void pddlMutexesPrintPython(const pddl_mutexes_t *ms, FILE *fout)
{
    int fact_id;

    fprintf(fout, "[\n");
    for (int i = 0; i < ms->size; ++i){
        const pddl_mutex_t *m = ms->m + i;
        fprintf(fout, "    {\n");
        fprintf(fout, "        'fact' : set([");
        BOR_ISET_FOR_EACH(&m->fact, fact_id)
            fprintf(fout, " %d,", fact_id);
        fprintf(fout, "]),\n");
        fprintf(fout, "        'hm' : %d,\n", m->hm);
        fprintf(fout, "    },\n");
    }
    fprintf(fout, "]\n");
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

void pddlMutexesPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                            const pddl_mutexes_t *ms, FILE *fout)
{
    pretty_t p;

    if (ms->size == 0)
        return;

    p.size = ms->size;
    p.m = BOR_ALLOC_ARR(int *, ms->size);
    for (int i = 0; i < ms->size; ++i){
        p.m[i] = BOR_ALLOC_ARR(int, ms->m[i].fact.size + 1);
        p.m[i][0] = ms->m[i].fact.size;
        for (int j = 0; j < ms->m[i].fact.size; ++j)
            p.m[i][j + 1] = borISetGet(&ms->m[i].fact, j);
        borSort(p.m[i] + 1, ms->m[i].fact.size, sizeof(int),
                prettyFactCmp, (void *)fs);
    }
    borSort(p.m, p.size, sizeof(int *), prettyMutexCmp, (void *)fs);

    for (int i = 0; i < p.size; ++i){
        fprintf(fout, "%d :: ", p.m[i][0]);
        for (int j = 0; j < p.m[i][0]; ++j){
            if (j > 0)
                fprintf(fout, "; ");
            fprintf(fout, "%s", fs->fact[p.m[i][j + 1]]->name);
        }
        fprintf(fout, "\n");
    }

    for (int i = 0; i < ms->size; ++i)
        BOR_FREE(p.m[i]);
    BOR_FREE(p.m);
}
