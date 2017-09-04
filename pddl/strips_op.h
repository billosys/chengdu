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

#ifndef __PDDL_STRIPS_OP_H__
#define __PDDL_STRIPS_OP_H__

#include <boruvka/htable.h>
#include <boruvka/iset.h>

#include <pddl/common.h>
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_strips_op_cond_eff {
    bor_iset_t pre;
    bor_iset_t del_eff;
    bor_iset_t add_eff;
};
typedef struct pddl_strips_op_cond_eff pddl_strips_op_cond_eff_t;

struct pddl_strips_op {
    char *name;
    int cost;
    bor_iset_t pre;
    bor_iset_t del_eff;
    bor_iset_t add_eff;
    pddl_strips_op_cond_eff_t *cond_eff;
    int cond_eff_size;
    int cond_eff_alloc;

    int id;
    uint64_t hash;
    bor_list_t htable;
};
typedef struct pddl_strips_op pddl_strips_op_t;

/**
 * Initializes strips operator.
 */
void pddlStripsOpInit(pddl_strips_op_t *op);
pddl_strips_op_t *pddlStripsOpNew(void);

/**
 * Frees allocated memory
 */
void pddlStripsOpFree(pddl_strips_op_t *op);
void pddlStripsOpDel(pddl_strips_op_t *op);

pddl_strips_op_cond_eff_t *pddlStripsOpAddCondEff(pddl_strips_op_t *op,
                                                  const pddl_strips_op_t *f);

/**
 * Makes the operator well-formed.
 */
void pddlStripsOpNormalize(pddl_strips_op_t *op);

/**
 * Finalizes strips operator after all parts, except name, is filled.
 */
int pddlStripsOpFinalize(pddl_strips_op_t *op, char *name);

/**
 * Adds all del and delete effects from src to dst.
 */
void pddlStripsOpAddEffFromOp(pddl_strips_op_t *dst,
                              const pddl_strips_op_t *src);

/**
 * Copy the operator including conditional effects.
 */
void pddlStripsOpCopy(pddl_strips_op_t *dst, const pddl_strips_op_t *src);

/**
 * Copy the operator without conditional effects.
 */
void pddlStripsOpCopyWithoutCondEff(pddl_strips_op_t *dst,
                                    const pddl_strips_op_t *src);

/**
 * Copy dual variant of the operator (i.e., exchanged pre and del_eff)
 */
void pddlStripsOpCopyDual(pddl_strips_op_t *dst, const pddl_strips_op_t *src);

/**
 * Remaps fact IDs according to the provided map (old ID -> new ID).
 * It is assumed that the mapping is monotonically increasing.
 */
void pddlStripsOpRemapFacts(pddl_strips_op_t *op, const int *remap);

/**
 * Remove the fact from preconditions and effects including conditional
 * effects. If the precondition of a conditional effect is made empty the
 * effects are merged with the ordinary effects.
 * The operator is kept well-formed.
 */
void pddlStripsOpRemoveFact(pddl_strips_op_t *op, int fact_id);



struct pddl_strips_ops {
    pddl_strips_op_t **op;
    int op_size;
    int op_alloc;
    bor_htable_t *htable;
};
typedef struct pddl_strips_ops pddl_strips_ops_t;

#define PDDL_STRIPS_OPS_FOR_EACH(OPS, OP) \
    for (int __i = 0; __i < (OPS)->op_size && ((OP) = (OPS)->op[__i], 1); \
            ++__i) \
        if ((OP) != NULL)

void pddlStripsOpsInit(pddl_strips_ops_t *ops);
void pddlStripsOpsFree(pddl_strips_ops_t *ops);

/**
 * Adds a new operator if not already added, operator add has to be
 * finalized by pddlStripsOpFinalize().
 */
int pddlStripsOpsAdd(pddl_strips_ops_t *ops, const pddl_strips_op_t *add);

/**
 * Deletes all operators for which m[op_id] is set to true.
 */
void pddlStripsOpsDelOps(pddl_strips_ops_t *ops, const int *m);

/**
 * Calls pddlStripsOpRemapFacts for each operator.
 */
void pddlStripsOpsRemapFacts(pddl_strips_ops_t *ops, const int *remap);

void pddlStripsOpPrint(const pddl_t *pddl, const pddl_facts_t *fs,
                       const pddl_strips_op_t *op, FILE *fout);
void pddlStripsOpsPrint(const pddl_t *pddl, const pddl_facts_t *fs,
                        const pddl_strips_ops_t *ops, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_OP_H__ */
