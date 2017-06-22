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

#ifndef __PDDL_FACT_ID_H__
#define __PDDL_FACT_ID_H__

#include <boruvka/alloc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;
struct pddl_facts;

/**
 * Set of fact IDs
 */
struct pddl_fact_id_set {
    int *fact;
    int size;
    int alloc;
};
typedef struct pddl_fact_id_set pddl_fact_id_set_t;

#define PDDL_FACT_ID_SET_FOR_EACH(fs, fact_id) \
    for (int __i = 0; \
            __i < (fs)->size && ((fact_id) = (fs)->fact[__i], 1); \
            ++__i)

/**
 * Initialize set of fact IDs
 */
_bor_inline void pddlFactIdSetInit(pddl_fact_id_set_t *s);

/**
 * Frees allocated memory.
 */
_bor_inline void pddlFactIdSetFree(pddl_fact_id_set_t *s);

/**
 * Returns true if fact_id belongs to s.
 */
int pddlFactIdSetHasId(const pddl_fact_id_set_t *s, int fact_id);

/**
 * Makes the set empty.
 */
_bor_inline void pddlFactIdSetEmpty(pddl_fact_id_set_t *s);

/**
 * Adds a new ID into the set if not already there.
 */
void pddlFactIdSetAdd(pddl_fact_id_set_t *s, int fact_id);

/**
 * Copies facts from src to dst; dst does not need to be empty.
 */
void pddlFactIdSetCopy(pddl_fact_id_set_t *dst, const pddl_fact_id_set_t *src);

/**
 * Allocate enough memory for size elements.
 * (It does not change s->size.)
 */
_bor_inline void pddlFactIdSetResize(pddl_fact_id_set_t *s, int size);

/**
 * Returns true if the sets are equal.
 */
_bor_inline int pddlFactIdSetEq(const pddl_fact_id_set_t *s1,
                                const pddl_fact_id_set_t *s2);

/**
 * s1 = s1 \setminus s2
 */
void pddlFactIdSetMinus(pddl_fact_id_set_t *s1, const pddl_fact_id_set_t *s2);

/**
 * s = s \setminus {fact_id}
 */
_bor_inline int pddlFactIdSetRmId(pddl_fact_id_set_t *s, int fact_id);

/**
 * Prints the set s as a sorted list of fact names.
 */
void pddlFactIdSetPrettyPrint(const struct pddl *pddl,
                              const struct pddl_facts *fs,
                              const pddl_fact_id_set_t *s,
                              FILE *fout);


/**
 * Set of sets (powerset) of fact IDs.
 */
struct pddl_fact_id_pset {
    pddl_fact_id_set_t *fact_set;
    int size;
    int alloc;
};
typedef struct pddl_fact_id_pset pddl_fact_id_pset_t;

/**
 * Initialize powerset of fact IDs.
 */
_bor_inline void pddlFactIdPSetInit(pddl_fact_id_pset_t *ps);

/**
 * Free all allocated memory.
 */
_bor_inline void pddlFactIdPSetFree(pddl_fact_id_pset_t *ps);

/**
 * Adds a new set to ps.
 */
void pddlFactIdPSetAdd(pddl_fact_id_pset_t *ps, const pddl_fact_id_set_t *s);



/**** INLINES: ****/
_bor_inline void pddlFactIdSetInit(pddl_fact_id_set_t *s)
{
    bzero(s, sizeof(*s));
}

_bor_inline void pddlFactIdSetFree(pddl_fact_id_set_t *s)
{
    if (s->fact)
        BOR_FREE(s->fact);
}

_bor_inline void pddlFactIdSetEmpty(pddl_fact_id_set_t *s)
{
    s->size = 0;
}

_bor_inline void pddlFactIdSetResize(pddl_fact_id_set_t *s, int size)
{
    s->fact = BOR_REALLOC_ARR(s->fact, int, size);
    s->size = size;
}

_bor_inline int pddlFactIdSetEq(const pddl_fact_id_set_t *s1,
                                const pddl_fact_id_set_t *s2)
{
    return s1->size == s2->size
            && memcmp(s1->fact, s2->fact, sizeof(int) * s1->size) == 0;
}

_bor_inline int pddlFactIdSetRmId(pddl_fact_id_set_t *s, int fact_id)
{
    int i;

    for (i = 0; i < s->size && s->fact[i] < fact_id; ++i);
    if (i < s->size && s->fact[i] == fact_id){
        for (++i; i < s->size; ++i)
            s->fact[i - 1] = s->fact[i];
        --s->size;
        return 1;
    }
    return 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FACT_ID_H__ */
