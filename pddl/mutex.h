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
#include <pddl/strips.h>

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
    pddl_mutex_t *m;
    int size;
    int alloc;
};
typedef struct pddl_mutexes pddl_mutexes_t;

#define PDDL_MUTEXES_FOR_EACH(MS, M) \
    for (int __i = 0; __i < (MS)->size && ((M) = (MS)->m + __i); ++__i)

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
 * Adds a new mutex consisting of the given facts.
 */
pddl_mutex_t *pddlMutexesAdd(pddl_mutexes_t *ms, const bor_iset_t *m);

/**
 * Finds h^m mutexes and store them in ms.
 * In unreachable_ops is non-NULL it is used as in/out map of operators:
 *   - only the operators with the false value are used
 *   - all unused (i.e., unreachable) operators are marked with true value
 * Does not work with conditional effects, but they can be compiled away.
 */
int pddlMutexesHm(int m, const pddl_strips_t *strips, pddl_mutexes_t *ms,
                  int *unreachable_ops);
pddl_mutexes_t *pddlMutexesHmNew(int m, const pddl_strips_t *strips,
                                 int *unreachable_ops);


void pddlMutexesPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                            const pddl_mutexes_t *ms, FILE *fout);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_MUTEX_H__ */
