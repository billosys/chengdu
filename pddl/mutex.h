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

#ifndef __PDDL_MUTEX_H__
#define __PDDL_MUTEX_H__

#include <boruvka/iset.h>

#include <pddl/common.h>
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Mutex -- a set of facts that cannot be subset of any reachable state.
 */
struct pddl_mutex {
    bor_iset_t fact; /*!< Set of fact IDs */
    int hm; /** If the mutex was obtained by h^m, the member contains m,
                otherwise 0 */
};
typedef struct pddl_mutex pddl_mutex_t;

/**
 * Set of mutexes.
 */
struct pddl_mutexes {
    pddl_mutex_t *mutex;
    int mutex_size;
    int mutex_alloc;

    int has_3; /*!< True if there are mutexes of size 3 and more */
    char *mutex2_map; /*!< Mapping from (fact x fact) to bool for mutexes
                           of size 2 and 1 */
    int mutex2_map_fact_size;
};
typedef struct pddl_mutexes pddl_mutexes_t;

#define PDDL_MUTEXES_FOR_EACH(MS, M) \
    for (int __i = 0; __i < (MS)->mutex_size \
                        && ((M) = (MS)->mutex + __i); ++__i)

void pddlMutexInit(pddl_mutex_t *m);
void pddlMutexFree(pddl_mutex_t *m);

/**
 * Initialize a new set of mutexes.
 */
void pddlMutexesInit(pddl_mutexes_t *ms);
pddl_mutexes_t *pddlMutexesNew(void);

/**
 * Free allocated memory.
 */
void pddlMutexesFree(pddl_mutexes_t *ms);
void pddlMutexesDel(pddl_mutexes_t *ms);

/**
 * Create a copy of the structure.
 */
void pddlMutexesCopy(pddl_mutexes_t *dst, const pddl_mutexes_t *src);

/**
 * Returns true if the conjuction of the given facts is mutex.
 */
int pddlMutexesIsMutex(const pddl_mutexes_t *ms, const bor_iset_t *facts);

/**
 * Returns true if f1 \cup f2 is mutex while assuming f1 nor f2 are
 * mutexes.
 */
int pddlMutexesIsMutex2(const pddl_mutexes_t *ms,
                        const bor_iset_t *f1,
                        const bor_iset_t *f2);

/**
 * Returns true if {fact} \cup f is a mutex.
 */
int pddlMutexesIsMutexWithFact(const pddl_mutexes_t *ms,
                               int fact, const bor_iset_t *f);

/**
 * Returns true if {fact1, fact2} is a mutex.
 */
int pddlMutexesIsMutexPair(const pddl_mutexes_t *ms, int fact1, int fact2);

/**
 * Adds a new mutex consisting of the given facts.
 */
pddl_mutex_t *pddlMutexesAdd(pddl_mutexes_t *ms, const bor_iset_t *m);

/**
 * Finds h^m mutexes and store them in ms.
 *   - mutexes: in/out parameter, the stored mutexes are used for
 *              initialization and the newly inferred mutexes are
 *              stored there.
 *   - m: parameter in h^m, currently supported only 2 and 3
 *   - strips: STRIPS problem
 *   - unreachable_ops: in/out parameter; if set to non-NULL, then only the
 *                      operators with the value set to false are used and
 *                      the operators that were not used are at the end set
 *                      to true.
 *   - max_mem/max_time: memory and time limits.
 * Does not work with conditional effects, but they can be compiled away.
 */
int pddlMutexesHm(pddl_mutexes_t *mutexes,
                  int m,
                  const pddl_strips_t *strips,
                  int *unreachable_ops,
                  size_t max_mem,
                  float max_time);

/**
 * Remove h^m mutexes larger than max_m.
 */
void pddlMutexesHmLimit(pddl_mutexes_t *ms, int max_m);

void pddlMutexesPrintPython(const pddl_mutexes_t *ms, FILE *fout);
void pddlMutexesPrettyPrint(const pddl_t *pddl, const pddl_facts_t *fs,
                            const pddl_mutexes_t *ms, FILE *fout);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_MUTEX_H__ */
