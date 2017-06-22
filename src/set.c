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

#include "pddl/set.h"

int pddlSetHasId(const pddl_set_t *s, int v)
{
    // TODO: binary search
    int i;
    PDDL_SET_FOR_EACH(s, i){
        if (i == v)
            return 1;
    }
    return 0;
}

void pddlSetAdd(pddl_set_t *s, int v)
{
    if (s->size >= s->alloc){
        if (s->alloc == 0)
            s->alloc = 1;
        s->alloc *= 2;
        s->v = BOR_REALLOC_ARR(s->v, int, s->alloc);
    }
    s->v[s->size++] = v;

    if (s->size > 1 && v < s->v[s->size - 2]){
        int *f = s->v + s->size - 1;
        for (; f > s->v && f[0] < f[-1]; --f){
            int tmp = f[0];
            f[0] = f[-1];
            f[-1] = tmp;
        }
        if (f > s->v && f[0] == f[-1]){
            for (--s->size; f != s->v + s->size; ++f)
                *f = f[1];
        }
    }
}

void pddlSetCopy(pddl_set_t *dst, const pddl_set_t *src)
{
    for (int i = 0; i < src->size; ++i)
        pddlSetAdd(dst, src->v[i]);
}

void pddlSetMinus(pddl_set_t *s1, const pddl_set_t *s2)
{
    int w, i, j;

    for (w = i = j = 0; i < s1->size && j < s2->size;){
        if (s1->v[i] == s2->v[j]){
            ++i;
            ++j;
        }else if (s1->v[i] < s2->v[j]){
            s1->v[w++] = s1->v[i++];
        }else{
            ++j;
        }
    }
    for (; i < s1->size; ++i, ++w)
        s1->v[w] = s1->v[i];
    s1->size = w + s1->size - i;
}
