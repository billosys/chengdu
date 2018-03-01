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

_bor_inline void resizeMutex2Map(pddl_mutexes_t *ms, int maxf)
{
    if (maxf < ms->mutex2_map_fact_size)
        return;

    char *old_map = ms->mutex2_map;
    int old_fact_size = ms->mutex2_map_fact_size;
    if (ms->mutex2_map_fact_size == 0)
        ms->mutex2_map_fact_size = 128;
    while (maxf >= ms->mutex2_map_fact_size)
        ms->mutex2_map_fact_size *= 2;
    ms->mutex2_map = BOR_CALLOC_ARR(char, ms->mutex2_map_fact_size
                                            * ms->mutex2_map_fact_size);
    if (old_map != NULL){
        for (int i = 0; i < old_fact_size; ++i){
            for (int j = i; j < old_fact_size; ++j){
                ms->mutex2_map[i * ms->mutex2_map_fact_size + j]
                    = ms->mutex2_map[j * ms->mutex2_map_fact_size + i]
                    = old_map[i * old_fact_size + j];
            }
        }
        BOR_FREE(old_map);
    }
}

static void updateMutex2Map_1(pddl_mutexes_t *ms, int fact)
{
    resizeMutex2Map(ms, fact);
    ms->mutex2_map[fact * ms->mutex2_map_fact_size + fact] = 1;

}

static void updateMutex2Map_2(pddl_mutexes_t *ms, int f1, int f2)
{
    resizeMutex2Map(ms, BOR_MAX(f1, f2));
    ms->mutex2_map[f1 * ms->mutex2_map_fact_size + f2] = 1;
    ms->mutex2_map[f2 * ms->mutex2_map_fact_size + f1] = 1;

}

static void updateMutex2Map(pddl_mutexes_t *ms, const bor_iset_t *m)
{
    if (borISetSize(m) == 1){
        updateMutex2Map_1(ms, borISetGet(m, 0));
    }else if (borISetSize(m) == 2){
        updateMutex2Map_2(ms, borISetGet(m, 0), borISetGet(m, 1));
    }
}

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
    if (ms->mutex != NULL)
        BOR_FREE(ms->mutex);
    if (ms->mutex2_map)
        BOR_FREE(ms->mutex2_map);
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

void pddlMutexesCopy(pddl_mutexes_t *dst, const pddl_mutexes_t *src)
{
    const pddl_mutex_t *sm;
    pddl_mutex_t *dm;

    PDDL_MUTEXES_FOR_EACH(src, sm){
        dm = pddlMutexesAdd(dst, &sm->fact);
        dm->hm = sm->hm;
    }
}

static int isMutex3(const pddl_mutexes_t *ms, const bor_iset_t *facts)
{
    const pddl_mutex_t *mutex;

    PDDL_MUTEXES_FOR_EACH(ms, mutex){
        if (borISetSize(&mutex->fact) >= 3
                && borISetIsSubset(&mutex->fact, facts))
            return 1;
    }
    return 0;
}

static int isMutex2WithFact(const pddl_mutexes_t *ms,
                            int fact1, const bor_iset_t *f)
{
    int fact2;

    if (fact1 >= ms->mutex2_map_fact_size)
        return 0;

    BOR_ISET_FOR_EACH(f, fact2){
        if (fact2 >= ms->mutex2_map_fact_size)
            return 0;
        if (ms->mutex2_map[fact1 * ms->mutex2_map_fact_size + fact2])
            return 1;
    }
    return 0;
}

int pddlMutexesIsMutex(const pddl_mutexes_t *ms, const bor_iset_t *facts)
{
    int size = borISetSize(facts);

    for (int i = 0; i < size; ++i){
        int f1 = borISetGet(facts, i);
        if (f1 >= ms->mutex2_map_fact_size)
            continue;

        for (int j = i; j < size; ++j){
            int f2 = borISetGet(facts, j);
            if (f2 >= ms->mutex2_map_fact_size)
                continue;

            if (ms->mutex2_map[f1 * ms->mutex2_map_fact_size + f2])
                return 1;
        }
    }

    if (ms->has_3)
        return isMutex3(ms, facts);
    return 0;
}

int pddlMutexesIsMutex2(const pddl_mutexes_t *ms,
                        const bor_iset_t *f1,
                        const bor_iset_t *f2)
{
    int fact1;

    BOR_ISET_FOR_EACH(f1, fact1){
        if (isMutex2WithFact(ms, fact1, f2))
            return 1;
    }

    if (ms->has_3){
        BOR_ISET(fs);
        borISetUnion2(&fs, f1, f2);
        int ret = isMutex3(ms, &fs);
        borISetFree(&fs);
        return ret;
    }

    return 0;
}

int pddlMutexesIsMutexWithFact(const pddl_mutexes_t *ms,
                               int fact, const bor_iset_t *f)
{
    return isMutex2WithFact(ms, fact, f);
}

int pddlMutexesIsMutexPair(const pddl_mutexes_t *ms, int fact1, int fact2)
{
    if (fact1 >= ms->mutex2_map_fact_size)
        return 0;
    if (fact2 >= ms->mutex2_map_fact_size)
        return 0;

    if (ms->mutex2_map[fact1 * ms->mutex2_map_fact_size + fact2])
        return 1;
    return 0;
}

pddl_mutex_t *pddlMutexesAdd(pddl_mutexes_t *ms, const bor_iset_t *m)
{
    if (ms->mutex_size >= ms->mutex_alloc){
        if (ms->mutex_alloc == 0)
            ms->mutex_alloc = 1;
        ms->mutex_alloc *= 2;
        ms->mutex = BOR_REALLOC_ARR(ms->mutex, pddl_mutex_t, ms->mutex_alloc);
    }

    pddlMutexInit(ms->mutex + ms->mutex_size);
    borISetUnion(&ms->mutex[ms->mutex_size].fact, m);
    if (borISetSize(&ms->mutex[ms->mutex_size].fact) >= 3){
        ms->has_3 = 1;
    }else{
        updateMutex2Map(ms, m);
    }
    ++ms->mutex_size;

    return ms->mutex + ms->mutex_size - 1;
}

void pddlMutexesPrintPython(const pddl_mutexes_t *ms, FILE *fout)
{
    const pddl_mutex_t *m;
    int fact_id;

    fprintf(fout, "[\n");
    PDDL_MUTEXES_FOR_EACH(ms, m){
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

    if (ms->mutex_size == 0)
        return;

    p.size = ms->mutex_size;
    p.m = BOR_ALLOC_ARR(int *, ms->mutex_size);
    for (int i = 0; i < ms->mutex_size; ++i){
        p.m[i] = BOR_ALLOC_ARR(int, ms->mutex[i].fact.size + 1);
        p.m[i][0] = ms->mutex[i].fact.size;
        for (int j = 0; j < ms->mutex[i].fact.size; ++j)
            p.m[i][j + 1] = borISetGet(&ms->mutex[i].fact, j);
        borSort(p.m[i] + 1, ms->mutex[i].fact.size, sizeof(int),
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

    for (int i = 0; i < ms->mutex_size; ++i)
        BOR_FREE(p.m[i]);
    BOR_FREE(p.m);
}
