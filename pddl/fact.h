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

#include <pddl/common.h>
#include <pddl/lisp.h>
#include <pddl/obj.h>
#include <pddl/pred.h>
#include <pddl/ground_atom.h>
#include <boruvka/iset.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PDDL_FACT_MAX_NAME_SIZE 256

struct pddl_fact {
    int id;
    bor_htable_key_t hash;
    bor_list_t htable;

    char *name; /*!< Name of the fact */
    pddl_ground_atom_t *ground_atom; /*!< If the fact was created from a
                                          grounded atom, its copy is stored
                                          here (may be NULL). */
};
typedef struct pddl_fact pddl_fact_t;

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
 * Compares two facts.
 */
int pddlFactCmp(const pddl_fact_t *f1, const pddl_fact_t *f2);


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
 * Initialize set of facts.
 */
void pddlFactsInit(pddl_facts_t *fs);

/**
 * Free allocated resources.
 */
void pddlFactsFree(pddl_facts_t *fs);

/**
 * Adds a new copy of the given fact.
 */
int pddlFactsAdd(pddl_facts_t *fs, const pddl_fact_t *f);

/**
 * Adds a fact created from the grounded atom.
 */
int pddlFactsAddGroundAtom(pddl_facts_t *fs, const pddl_ground_atom_t *ga,
                           const pddl_t *pddl);

/**
 * Deletes fact (and frees all its memory).
 */
void pddlFactsDelFact(pddl_facts_t *fs, int fact_id);

/**
 * Deletes the facts marked in the m array.
 * Returns remap array containing remapping of fact IDs (old ID -> new ID)
 * that is monotonically increasing, except for the facts that were removed
 * where the new ID is set to -1.
 */
void pddlFactsDelIrrelevantFacts(pddl_facts_t *fs, const int *m, int *remap);

/**
 * Copies fact from src to dst.
 */
void pddlFactsCopy(pddl_facts_t *dst, const pddl_facts_t *src);

/**
 * Sorts facts by their name and returns remapping from the old id to the
 * new id if remap is non-NULL.
 */
void pddlFactsSort(pddl_facts_t *fs, int *remap);


void pddlFactsPrintSorted(const pddl_facts_t *fs,
                          const char *prefix, const char *suffix,
                          FILE *fout);
void pddlFactsIdSetPrintSorted(const bor_iset_t *set,
                               const pddl_facts_t *fs,
                               const char *prefix, const char *suffix,
                               FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_FACT_H__ */
