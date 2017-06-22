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
#include <boruvka/htable.h>
#include <pddl/lisp.h>
#include <pddl/obj.h>
#include <pddl/pred.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;
struct pddl_facts;

struct pddl_fact_id_set {
    int *fact;
    int size;
    int alloc;
};
typedef struct pddl_fact_id_set pddl_fact_id_set_t;

_bor_inline void pddlFactIdSetInit(pddl_fact_id_set_t *arr);
_bor_inline void pddlFactIdSetFree(pddl_fact_id_set_t *arr);
void pddlFactIdSetAdd(pddl_fact_id_set_t *arr, int fact_id);
void pddlFactIdSetCopy(pddl_fact_id_set_t *dst, const pddl_fact_id_set_t *src);
_bor_inline void pddlFactIdSetResize(pddl_fact_id_set_t *arr, int size);
_bor_inline int pddlFactIdSetEq(const pddl_fact_id_set_t *a1,
                                const pddl_fact_id_set_t *a2);
/**
 * a1 = a1 \setminus a2
 * assuming both a1 and a2 are sorted
 */
void pddlFactIdSetMinus(pddl_fact_id_set_t *a1, const pddl_fact_id_set_t *a2);
_bor_inline int pddlFactIdSetRmId(pddl_fact_id_set_t *a, int fact_id);
void pddlFactIdSetPrettyPrint(const struct pddl *pddl,
                              const struct pddl_facts *fs,
                              const pddl_fact_id_set_t *arr, FILE *fout);


/**** INLINES: ****/
_bor_inline void pddlFactIdSetInit(pddl_fact_id_set_t *arr)
{
    bzero(arr, sizeof(*arr));
}

_bor_inline void pddlFactIdSetFree(pddl_fact_id_set_t *arr)
{
    if (arr->fact)
        BOR_FREE(arr->fact);
}

_bor_inline void pddlFactIdSetResize(pddl_fact_id_set_t *arr, int size)
{
    arr->fact = BOR_REALLOC_ARR(arr->fact, int, size);
    arr->size = size;
}

_bor_inline int pddlFactIdSetEq(const pddl_fact_id_set_t *a1,
                                const pddl_fact_id_set_t *a2)
{
    return a1->size == a2->size
            && memcmp(a1->fact, a2->fact, sizeof(int) * a1->size) == 0;
}

_bor_inline int pddlFactIdSetRmId(pddl_fact_id_set_t *a, int fact_id)
{
    int i;

    for (i = 0; i < a->size && a->fact[i] < fact_id; ++i);
    if (i < a->size && a->fact[i] == fact_id){
        for (++i; i < a->size; ++i)
            a->fact[i - 1] = a->fact[i];
        --a->size;
        return 1;
    }
    return 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FACT_ID_H__ */
