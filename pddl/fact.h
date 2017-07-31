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
#include <boruvka/iset.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;

struct pddl_fact {
    int *arg;       /*!< Object IDs are arguments */
    int arg_size;   /*!< Number of arguments */
    int pred;       /*!< Predicate ID */
    int func_val;   /*!< Assigned value in case of function */
    int is_private; /*!< True if the fact is private */
    int owner;      /*!< Owner object ID in case the fact is private */

    int id;
    uint64_t hash;
    bor_list_t htable;
};
typedef struct pddl_fact pddl_fact_t;

/**
 * Allocates fact struct on stack with enough space in .arg[] for any
 * grounded atom.
 */
#define PDDL_FACT_FOR_GROUND(PDDL, F_NAME) \
    pddl_fact_t F_NAME; \
    int F_NAME ## __args__[pddlPredFuncMaxParamSize(PDDL)]; \
    pddlFactInit(&F_NAME); \
    F_NAME.arg = F_NAME ## __args__

#define PDDL_FACT_FOR_GROUND2(F_NAME, ARG_SIZE) \
    pddl_fact_t F_NAME; \
    int F_NAME ## __args__[ARG_SIZE]; \
    pddlFactInit(&F_NAME); \
    F_NAME.arg = F_NAME ## __args__

/**
 * Initializes empty fact.
 */
void pddlFactInit(pddl_fact_t *f);
pddl_fact_t *pddlFactNew(void);

/**
 * Frees allocated memory
 */
void pddlFactFree(pddl_fact_t *f);
void pddlFactDel(pddl_fact_t *f);

/**
 * Deep copy of the fact.
 */
void pddlFactCopy(pddl_fact_t *dst, const pddl_fact_t *src);

/**
 * Compares two facts.
 */
int pddlFactCmp(const pddl_fact_t *f1, const pddl_fact_t *f2);

/**
 * Returns true if facts are equal.
 */
int pddlFactEq(const pddl_fact_t *f1, const pddl_fact_t *f2);

/**
 * Determines whether the fact should be private and which object should be
 * owner.
 * Returns 0 if fact remained non-private, 1 if privateness was set and -1
 * if there is conflict in owners of the fact (thus this is invalid fact).
 */
int pddlFactSetPrivate(const struct pddl *pddl, pddl_fact_t *fact);

/**
 * Returns formatted name of the fact.
 * The returned value is a static variable managed inside this module -- do
 * not free it!
 */
const char *pddlFactToStr(const struct pddl *pddl, const pddl_fact_t *f);

/**
 * Print formatted fact/func.
 */
void pddlFactPrint(const struct pddl *p, const pddl_fact_t *f, FILE *fout);
void pddlFuncPrint(const struct pddl *p, const pddl_fact_t *f, FILE *fout);

/**
 * Returns true if the fact is static.
 */
int pddlFactIsStatic(const struct pddl *pddl, const pddl_fact_t *f);



struct pddl_facts {
    pddl_fact_t **fact;
    int fact_size;
    int fact_alloc;
    bor_htable_t *htable;
};
typedef struct pddl_facts pddl_facts_t;

#define PDDL_FACTS_FOR_EACH(FACTS, FACT) \
    for (int __i = 0; \
            __i < (FACTS)->fact_size && ((FACT) = (FACTS)->fact[__i], 1); \
            ++__i) \
        if ((FACT) != NULL)

/**
 * Parses :init into list of instantiated predicates and instantiated
 * functions.
 */
int pddlFactsParseInit(struct pddl *pddl);

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
 * Deletes fact (and frees all its memory).
 */
void pddlFactsDelFact(pddl_facts_t *fs, int fact_id);

/**
 * Reallocate array so that .alloc == .size.
 */
void pddlFactsSqueeze(pddl_facts_t *fs);

/**
 * Copies fact from src to dst.
 */
void pddlFactsCopy(pddl_facts_t *dst, const pddl_facts_t *src);

/**
 * Returns ID of the fact.
 */
int pddlFactsFind(const pddl_facts_t *fs, const pddl_fact_t *f);

void pddlFactsPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                    FILE *fout);
void pddlFuncsPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                    FILE *fout);
void pddlFactsPrintSorted(const struct pddl *pddl,
                          const pddl_facts_t *fs,
                          FILE *fout);
void pddlFuncsPrintSorted(const struct pddl *pddl,
                          const pddl_facts_t *fs,
                          FILE *fout);
void pddlFactsPrintInit(const struct pddl *pddl,
                        const pddl_facts_t *in,
                        FILE *fout);
void pddlFactsPrintInitFunc(const struct pddl *pddl,
                            const pddl_facts_t *in,
                            FILE *fout);

void pddlFactIdSetPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                              const bor_iset_t *s, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FACT_H__ */
