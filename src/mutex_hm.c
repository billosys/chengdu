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
#include "pddl/strips.h"
#include "pddl/mutex.h"
#include "err.h"

/** Implemented in src/mutex_h2.c */
int _pddlMutexesH2(pddl_mutexes_t *ms,
                   const pddl_strips_t *strips,
                   int *unreachable_ops,
                   size_t max_mem,
                   float max_time);
/** Implemented in src/mutex_h3.c */
int _pddlMutexesH3(pddl_mutexes_t *ms,
                   const pddl_strips_t *strips,
                   int *unreachable_ops,
                   size_t max_mem,
                   float max_time);

int pddlMutexesHm(pddl_mutexes_t *ms,
                  int m,
                  const pddl_strips_t *strips,
                  int *unreachable_ops,
                  size_t max_mem,
                  float max_time)
{
    if (m != 2 && m != 3)
        ERR_RET2(-1, "pddlMutexesHm() is implemented only for h^2 for now.");
    if (m == 2)
        return _pddlMutexesH2(ms, strips, unreachable_ops, max_mem, max_time);
    if (m == 3)
        return _pddlMutexesH3(ms, strips, unreachable_ops, max_mem, max_time);
    return -1;
}

void pddlMutexesHmLimit(pddl_mutexes_t *ms, int max_m)
{
    int ins;

    ins = 0;
    for (int i = 0; i < ms->mutex_size; ++i){
        if (ms->mutex[i].hm && borISetSize(&ms->mutex[i].fact) > max_m){
            pddlMutexFree(&ms->mutex[i]);
        }else{
            ms->mutex[ins++] = ms->mutex[i];
        }
    }
    ms->mutex_size = ins;
}
