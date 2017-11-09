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
int _pddlMutexesH2(const pddl_strips_t *strips, pddl_mutexes_t *ms,
                   int *unreachable_ops);

int pddlMutexesHm(int m, const pddl_strips_t *strips, pddl_mutexes_t *ms,
                  int *unreachable_ops)
{
    if (m != 2)
        ERR_RET2(-1, "pddlMutexesHm() is implemented only for h^2 for now.");
    if (m == 2)
        return _pddlMutexesH2(strips, ms, unreachable_ops);
    return -1;
}

pddl_mutexes_t *pddlMutexesHmNew(int m, const pddl_strips_t *strips,
                                 int *unreachable_ops)
{
    pddl_mutexes_t *ms = pddlMutexesNew();
    if (pddlMutexesHm(m, strips, ms, unreachable_ops) != 0){
        pddlMutexesDel(ms);
        TRACE_RET(NULL);
    }
    return ms;
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
