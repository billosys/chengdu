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

#ifndef __PDDL_STRIPS_FACT_H__
#define __PDDL_STRIPS_FACT_H__

#include <boruvka/htable.h>
#include <pddl/pddl.h>
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Flags: **/
#define PDDL_STRIPS_GROUND_FULL 0x1u
#define PDDL_STRIPS_GROUND_NAIVE 0x2u

struct pddl_strips_fact {
    const char *name;
    int neg;        /*!< True if it is negated form */
    int stat;       /*!< True if the fact is static */
    int is_private; /*!< True if the fact is private */
    int owner;      /*!< Owner ID in case the fact is private */
};
typedef struct pddl_strips_fact pddl_strips_fact_t;

struct pddl_strips_facts {
    pddl_strips_fact_t *fact;
    int fact_size;
    int fact_alloc;
    bor_htable_t *htable;
};
typedef struct pddl_strips_facts pddl_strips_facts_t;


void pddlStripsFactsInit(pddl_strips_facts_t *fs);
void pddlStripsFactsFree(pddl_strips_facts_t *fs);
int pddlStripsFactsAddFromPDDLFact(pddl_strips_facts_t *fs,
                                   const pddl_t *pddl,
                                   const pddl_fact_t *fact);
int pddlStripsFactsFind(pddl_strips_facts_t *fs, const char *name);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_FACT_H__ */
