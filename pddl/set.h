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

#ifndef __PDDL_SET_H__
#define __PDDL_SET_H__

#include <boruvka/compiler.h>
#include <boruvka/alloc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Set of fact IDs
 */
struct pddl_set {
    int *v;
    int size;
    int alloc;
};
typedef struct pddl_set pddl_set_t;

#define PDDL_SET_FOR_EACH(S, V) \
    for (int __i = 0; __i < (S)->size && ((V) = (S)->v[__i], 1); ++__i)

/**
 * Initialize the set.
 */
_bor_inline void pddlSetInit(pddl_set_t *s);

/**
 * Frees allocated memory.
 */
_bor_inline void pddlSetFree(pddl_set_t *s);

/**
 * Returns true if the set contains val.
 */
int pddlSetHas(const pddl_set_t *s, int val);

/**
 * Makes the set empty.
 */
_bor_inline void pddlSetEmpty(pddl_set_t *s);

/**
 * Adds a new value into the set if not already there.
 */
void pddlSetAdd(pddl_set_t *s, int val);

/**
 * Copies values from src to dst; dst does not need to be empty.
 */
void pddlSetCopy(pddl_set_t *dst, const pddl_set_t *src);

/**
 * Allocate enough memory for size elements.
 * (It does not change s->size.)
 */
_bor_inline void pddlSetResize(pddl_set_t *s, int size);

/**
 * Returns true if the sets are equal.
 */
_bor_inline int pddlSetEq(const pddl_set_t *s1, const pddl_set_t *s2);

/**
 * s1 = s1 \setminus s2
 */
void pddlSetMinus(pddl_set_t *s1, const pddl_set_t *s2);

/**
 * s = s \setminus {val}
 */
_bor_inline int pddlSetRm(pddl_set_t *s, int val);



/**
 * Set of sets (powerset) of fact IDs.
 */
struct pddl_pset {
    pddl_set_t *set;
    int size;
    int alloc;
};
typedef struct pddl_pset pddl_pset_t;

/**
 * Initialize the powerset.
 */
_bor_inline void pddlPSetInit(pddl_pset_t *ps);

/**
 * Free all allocated memory.
 */
_bor_inline void pddlPSetFree(pddl_pset_t *ps);

/**
 * Adds a new set to ps.
 */
void pddlPSetAdd(pddl_pset_t *ps, const pddl_set_t *s);



/**** INLINES: ****/
_bor_inline void pddlSetInit(pddl_set_t *s)
{
    bzero(s, sizeof(*s));
}

_bor_inline void pddlSetFree(pddl_set_t *s)
{
    if (s->v)
        BOR_FREE(s->v);
}

_bor_inline void pddlSetEmpty(pddl_set_t *s)
{
    s->size = 0;
}

_bor_inline void pddlSetResize(pddl_set_t *s, int size)
{
    s->v = BOR_REALLOC_ARR(s->v, int, size);
    s->size = size;
}

_bor_inline int pddlSetEq(const pddl_set_t *s1, const pddl_set_t *s2)
{
    return s1->size == s2->size
            && memcmp(s1->v, s2->v, sizeof(int) * s1->size) == 0;
}

_bor_inline int pddlSetRm(pddl_set_t *s, int v)
{
    int i;

    for (i = 0; i < s->size && s->v[i] < v; ++i);
    if (i < s->size && s->v[i] == v){
        for (++i; i < s->size; ++i)
            s->v[i - 1] = s->v[i];
        --s->size;
        return 1;
    }
    return 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_SET_H__ */
