/***
 * cpddl
 * -------
 * Copyright (c)2016 Daniel Fiser <danfis@danfis.cz>,
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

#ifndef __PDDL_FACT_H__
#define __PDDL_FACT_H__

#include <boruvka/alloc.h>
#include <boruvka/htable.h>
#include <pddl/lisp.h>
#include <pddl/obj.h>
#include <pddl/pred.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;

struct pddl_fact {
    int *arg;       /*!< Object IDs are arguments */
    int arg_size;   /*!< Number of arguments */
    int pred;       /*!< Predicate ID */
    int neg;        /*!< True if it is negated form */
    int func_val;   /*!< Assigned value in case of function */
    int stat;       /*!< True if the fact is static */
    int is_private; /*!< True if the fact is private */
    int owner;      /*!< Owner object ID in case the fact is private */
};
typedef struct pddl_fact pddl_fact_t;

/**
 * Initializes empty fact.
 */
void pddlFactInit(pddl_fact_t *f);

/**
 * Initializes fact from the predicate.
 */
void pddlFactFromPred(pddl_fact_t *f, int pred_id, const pddl_pred_t *pred);

/**
 * Frees allocated memory
 */
void pddlFactFree(pddl_fact_t *f);

/**
 * Deep copy of the fact.
 */
void pddlFactCopy(pddl_fact_t *dst, const pddl_fact_t *src);

/**
 * Comparison function for facts.
 */
int pddlFactCmp(const pddl_fact_t *f1, const pddl_fact_t *f2);

/**
 * Determines whether the fact should be private and which object should be
 * owner.
 * Returns 0 if fact remained non-private, 1 if privateness was set and -1
 * if there is conflict in owners of the fact (thus this is invalid fact).
 */
int pddlFactSetPrivate(pddl_fact_t *fact,
                       const pddl_preds_t *pred,
                       const pddl_objs_t *objs);

int pddlFactFormat(const pddl_preds_t *predicates,
                   const pddl_objs_t *objs,
                   const pddl_fact_t *f,
                   char *str,
                   int strsize);

void pddlFactPrint(const pddl_preds_t *predicates,
                   const pddl_objs_t *objs,
                   const pddl_fact_t *f,
                   FILE *fout);



struct pddl_facts {
    pddl_fact_t *fact;
    int fact_size;
    int fact_alloc;
    bor_htable_t *htable;
};
typedef struct pddl_facts pddl_facts_t;


/**
 * Parses :init into list of instantiated predicates and instantiated
 * functions.
 */
int pddlFactsParseInit(struct pddl *pddl);

/**
 * Parses :goal into list of facts.
 */
int pddlFactsParseGoal(const pddl_lisp_t *problem,
                       const pddl_preds_t *predicates,
                       const pddl_objs_t *objs,
                       pddl_facts_t *goal);

/**
 * Initialize set of facts.
 */
void pddlFactsInit(pddl_facts_t *fs);

/**
 * Free allocated resources.
 */
void pddlFactsFree(pddl_facts_t *fs);

/**
 * Adds another fact to array.
 */
int pddlFactsAdd(pddl_facts_t *fs, const pddl_fact_t *f);

/**
 * Reallocate array so that .alloc == .size.
 */
void pddlFactsSqueeze(pddl_facts_t *fs);

/**
 * Reserve at least alloc members in array.
 */
void pddlFactsReserve(pddl_facts_t *fs, int alloc);

/**
 * Copies fact from src to dst.
 */
void pddlFactsCopy(pddl_facts_t *dst, const pddl_facts_t *src);

/**
 * Returns ID of the fact.
 */
int pddlFactsFind(pddl_facts_t *fs, const pddl_fact_t *f);

void pddlFactsPrintInit(const pddl_preds_t *predicates,
                        const pddl_objs_t *objs,
                        const pddl_facts_t *in,
                        FILE *fout);
void pddlFactsPrintInitFunc(const pddl_preds_t *predicates,
                            const pddl_objs_t *objs,
                            const pddl_facts_t *in,
                            FILE *fout);
void pddlFactsPrintGoal(const pddl_preds_t *predicates,
                        const pddl_objs_t *objs,
                        const pddl_facts_t *in,
                        FILE *fout);

struct pddl_fact_id_arr {
    int *fact;
    int size;
};
typedef struct pddl_fact_id_arr pddl_fact_id_arr_t;

_bor_inline void pddlFactIdArrInit(pddl_fact_id_arr_t *arr);
_bor_inline void pddlFactIdArrFree(pddl_fact_id_arr_t *arr);
_bor_inline void pddlFactIdArrResize(pddl_fact_id_arr_t *arr, int size);


/**** INLINES: ****/
_bor_inline void pddlFactIdArrInit(pddl_fact_id_arr_t *arr)
{
    arr->fact = NULL;
    arr->size = 0;
}

_bor_inline void pddlFactIdArrResize(pddl_fact_id_arr_t *arr, int size)
{
    arr->fact = BOR_REALLOC_ARR(arr->fact, int, size);
    arr->size = size;
}

_bor_inline void pddlFactIdArrFree(pddl_fact_id_arr_t *arr)
{
    if (arr->fact)
        BOR_FREE(arr->fact);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FACT_H__ */
