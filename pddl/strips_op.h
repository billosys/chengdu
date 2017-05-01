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
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;

struct pddl_strips_op_cond_eff {
    pddl_fact_id_arr_t pre;
    pddl_fact_id_arr_t del_eff;
    pddl_fact_id_arr_t add_eff;
};
typedef struct pddl_strips_op_cond_eff pddl_strips_op_cond_eff_t;

struct pddl_strips_op {
    char *name;
    int cost;
    pddl_fact_id_arr_t pre;
    pddl_fact_id_arr_t del_eff;
    pddl_fact_id_arr_t add_eff;
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

/**
 * Adds fact-id as precondition, add effect, or del effect, respectivelly.
 */
_bor_inline void pddlStripsOpAddPre(pddl_strips_op_t *op, int fact_id)
{
    pddlFactIdArrAdd(&op->pre, fact_id);
}
_bor_inline void pddlStripsOpAddAddEff(pddl_strips_op_t *op, int fact_id)
{
    pddlFactIdArrAdd(&op->add_eff, fact_id);
}
_bor_inline void pddlStripsOpAddDelEff(pddl_strips_op_t *op, int fact_id)
{
    pddlFactIdArrAdd(&op->del_eff, fact_id);
}

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
 * Copy dual variant of the operator (i.e., exchanged pre and del_eff)
 */
void pddlStripsOpCopyDual(pddl_strips_op_t *dst, const pddl_strips_op_t *src);

_bor_inline int pddlStripsOpRmFactId(pddl_strips_op_t *op, int fact_id)
{
    return pddlFactIdArrRmId(&op->pre, fact_id)
            | pddlFactIdArrRmId(&op->add_eff, fact_id)
            | pddlFactIdArrRmId(&op->del_eff, fact_id);
}

_bor_inline int pddlStripsOpRmFactIdFromDelEff(pddl_strips_op_t *op, int fid)
{
    return pddlFactIdArrRmId(&op->del_eff, fid);
}

_bor_inline int pddlStripsOpRmFactIdFromAddEff(pddl_strips_op_t *op, int fid)
{
    return pddlFactIdArrRmId(&op->add_eff, fid);
}

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

_bor_inline void pddlStripsOpsRmFactId(pddl_strips_ops_t *ops, int fact_id)
{
    pddl_strips_op_t *op;

    PDDL_STRIPS_OPS_FOR_EACH(ops, op){
        pddlStripsOpRmFactId(op, fact_id);
    }
}

_bor_inline void pddlStripsOpsRmFactIdFromDelEff(pddl_strips_ops_t *ops, int id)
{
    pddl_strips_op_t *op;

    PDDL_STRIPS_OPS_FOR_EACH(ops, op){
        pddlStripsOpRmFactIdFromDelEff(op, id);
    }
}

_bor_inline void pddlStripsOpsRmFactIdFromAddEff(pddl_strips_ops_t *ops, int id)
{
    pddl_strips_op_t *op;

    PDDL_STRIPS_OPS_FOR_EACH(ops, op){
        pddlStripsOpRmFactIdFromAddEff(op, id);
    }
}

void pddlStripsOpPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                       const pddl_strips_op_t *op, FILE *fout);
void pddlStripsOpsPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                        const pddl_strips_ops_t *ops, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_OP_H__ */
