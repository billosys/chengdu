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

#include <boruvka/alloc.h>

#include "pddl/fdr_op.h"
#include "err.h"
#include "assert.h"

static int stripsPreToFDRPre(pddl_fdr_part_state_t *fdr_pre,
                             const bor_iset_t *strips_pre,
                             const pddl_fdr_vars_t *vars)
{
    const pddl_fdr_val_t *val;
    int fact_id;

    pddlFDRPartStateInit(fdr_pre, vars);

    BOR_ISET_FOR_EACH(strips_pre, fact_id){
        ASSERT(vars->strips_fact_id_to_val[fact_id] != NULL);
        val = vars->strips_fact_id_to_val[fact_id];
        if (pddlFDRPartStateSet(fdr_pre, val->var_id, val->val_id) != 0)
            TRACE_RET(-1);
    }

    return 0;
}

static int stripsEffToFDREff(pddl_fdr_part_state_t *fdr_eff,
                             const bor_iset_t *strips_add_eff,
                             const bor_iset_t *strips_del_eff,
                             const pddl_fdr_vars_t *vars)
{
    const pddl_fdr_val_t *val;
    int fact_id;

    pddlFDRPartStateInit(fdr_eff, vars);

    // First set add effects
    BOR_ISET_FOR_EACH(strips_add_eff, fact_id){
        ASSERT(vars->strips_fact_id_to_val[fact_id] != NULL);
        val = vars->strips_fact_id_to_val[fact_id];
        if (pddlFDRPartStateSet(fdr_eff, val->var_id, val->val_id) != 0)
            TRACE_RET(-1);
    }

    // and then find delete effects that are not set and set the
    // corresponding variable to "none of those"
    BOR_ISET_FOR_EACH(strips_del_eff, fact_id){
        ASSERT(vars->strips_fact_id_to_val[fact_id] != NULL);
        val = vars->strips_fact_id_to_val[fact_id];
        if (pddlFDRPartStateIsSet(fdr_eff, val->var_id))
            continue;

        // Get "none of those" value which is the 
        ASSERT(vars->var[val->var_id].none_of_those != NULL);
        val = vars->var[val->var_id].none_of_those;
        if (pddlFDRPartStateSet(fdr_eff, val->var_id, val->val_id) != 0)
            TRACE_RET(-1);
    }

    return 0;
}

int pddlFDROpInit(pddl_fdr_op_t *op,
                  int id,
                  const pddl_fdr_vars_t *vars,
                  const pddl_strips_op_t *strips_op)
{
    bzero(op, sizeof(*op));
    op->id = id;
    op->strips_op_id = strips_op->id;
    if (strips_op->name != NULL)
        op->name = BOR_STRDUP(strips_op->name);
    op->cost = strips_op->cost;

    if (stripsPreToFDRPre(&op->pre, &strips_op->pre, vars) != 0)
        TRACE_RET(-1);
    if (stripsEffToFDREff(&op->eff, &strips_op->add_eff,
                          &strips_op->del_eff, vars) != 0){
        TRACE_RET(-1);
    }

    if (strips_op->cond_eff_size == 0)
        return 0;

    op->cond_eff_alloc = op->cond_eff_size = strips_op->cond_eff_size;
    op->cond_eff = BOR_CALLOC_ARR(pddl_fdr_cond_eff_t, op->cond_eff_size);
    for (int cei = 0; cei < op->cond_eff_size; ++cei){
        if (stripsPreToFDRPre(&op->cond_eff[cei].pre,
                              &strips_op->cond_eff[cei].pre, vars) != 0){
            TRACE_RET(-1);
        }
        if (stripsEffToFDREff(&op->cond_eff[cei].eff,
                              &strips_op->cond_eff[cei].add_eff,
                              &strips_op->cond_eff[cei].del_eff, vars) != 0){
            TRACE_RET(-1);
        }
    }

    return 0;
}

void pddlFDROpFree(pddl_fdr_op_t *op)
{
    if (op->name != NULL)
        BOR_FREE(op->name);
    pddlFDRPartStateFree(&op->pre);
    pddlFDRPartStateFree(&op->eff);
    for (int i = 0; i < op->cond_eff_size; ++i){
        pddlFDRPartStateFree(&op->cond_eff[i].pre);
        pddlFDRPartStateFree(&op->cond_eff[i].eff);
    }
    if (op->cond_eff != NULL)
        BOR_FREE(op->cond_eff);
}

void pddlFDROpPrettyPrint(const pddl_fdr_op_t *op,
                          const pddl_fdr_vars_t *vars,
                          FILE *fout)
{
    fprintf(fout, "  (%s), cost: %d\n", op->name, op->cost);
    fprintf(fout, "    pre:");
    pddlFDRPartStatePrettyPrint(&op->pre, vars, fout);
    fprintf(fout, "    eff:");
    pddlFDRPartStatePrettyPrint(&op->eff, vars, fout);

    if (op->cond_eff_size > 0){
        fprintf(fout, "    cond_eff[%d]:\n", op->cond_eff_size);
        for (int i = 0; i < op->cond_eff_size; ++i){
            fprintf(fout, "      pre:");
            pddlFDRPartStatePrettyPrint(&op->cond_eff[i].pre, vars, fout);
            fprintf(fout, "      eff:");
            pddlFDRPartStatePrettyPrint(&op->cond_eff[i].eff, vars, fout);
        }
    }
}
