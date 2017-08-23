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

#include "pddl/fdr.h"
#include "assert.h"

static void setUnsolvable(pddl_fdr_t *fdr)
{
    // Keep the variables and set a bogus initial state
    for (int i = 0; i < fdr->var.size; ++i)
        pddlFDRPartStateSet(&fdr->init, i, 0);
    // and a different goal
    pddlFDRPartStateSet(&fdr->goal, 0, 1);
}

pddl_fdr_t *pddlFDRFromStrips(const pddl_strips_t *strips,
                              const pddl_mgroups_t *mgroups,
                              unsigned vars_flags)
{
    pddl_fdr_t *fdr;
    const pddl_fdr_val_t *val;
    int fact_id;

    fdr = BOR_ALLOC(pddl_fdr_t);
    bzero(fdr, sizeof(*fdr));
    fdr->strips = strips;
    pddlFDRVarsInit(&fdr->var, strips, mgroups, vars_flags);
    pddlFDRPartStateInit(&fdr->goal, &fdr->var);
    pddlFDRPartStateInit(&fdr->init, &fdr->var);

    // If goal cannot be transformed, the problem is unsolvable, so create
    // degenrated problem without operators and incompatible init and goal
    BOR_ISET_FOR_EACH(&strips->goal, fact_id){
        val = fdr->var.strips_fact_id_to_val[fact_id];
        if (pddlFDRPartStateSet(&fdr->goal, val->var_id, val->val_id) != 0){
            // reset init and goal
            pddlFDRPartStateFree(&fdr->init);
            pddlFDRPartStateInit(&fdr->init, &fdr->var);
            pddlFDRPartStateFree(&fdr->goal);
            pddlFDRPartStateInit(&fdr->goal, &fdr->var);

            // set the problem unsolvable
            setUnsolvable(fdr);
            return fdr;
        }
    }

    // Translate initial state -- this should not fail!
    BOR_ISET_FOR_EACH(&strips->init, fact_id){
        val = fdr->var.strips_fact_id_to_val[fact_id];
        ASSERT(pddlFDRPartStateSet(&fdr->init, val->var_id, val->val_id) == 0);
        pddlFDRPartStateSet(&fdr->init, val->var_id, val->val_id);
    }
    // Replace -1 with "none of those"
    for (int i = 0; i < fdr->var.size; ++i){
        if (!pddlFDRPartStateIsSet(&fdr->init, i)){
            ASSERT(fdr->var.var[i].none_of_those != NULL);
            pddlFDRPartStateSet(&fdr->init, i,
                                fdr->var.var[i].none_of_those->val_id);
        }
    }

    fdr->op = BOR_ALLOC_ARR(pddl_fdr_op_t, strips->op.op_size);
    fdr->op_size = 0;
    for (int i = 0; i < strips->op.op_size; ++i){
        // Operators that cannot be transformed can be skipped
        //   -- this can happen in the case when pruning is not ran on the
        //   STRIPS problem, i.e., the invariants, from which the variables
        //   are created, are in conflict with some operator.
        if (pddlFDROpInit(fdr->op + fdr->op_size, fdr->op_size,
                          &fdr->var, strips->op.op[i]) == 0){
            ++fdr->op_size;
        }else{
            pddlFDROpFree(fdr->op + fdr->op_size);
        }
    }

    return fdr;
}

void pddlFDRDel(pddl_fdr_t *fdr)
{
    pddlFDRPartStateFree(&fdr->goal);
    pddlFDRPartStateFree(&fdr->init);
    for (int i = 0; i < fdr->op_size; ++i)
        pddlFDROpFree(fdr->op + i);
    if (fdr->op != NULL)
        BOR_FREE(fdr->op);
    pddlFDRVarsFree(&fdr->var);
    BOR_FREE(fdr);
}

void pddlFDRPrettyPrint(const pddl_fdr_t *fdr, FILE *fout)
{
    fprintf(fout, "Vars[%d]:\n", fdr->var.size);
    pddlFDRVarsPrint(&fdr->var, fout);
    fprintf(fout, "Ops[%d]:\n", fdr->op_size);
    for (int i = 0; i < fdr->op_size; ++i){
        pddlFDROpPrettyPrint(fdr->op + i, &fdr->var, fout);
    }
    fprintf(fout, "Init:");
    pddlFDRPartStatePrettyPrint(&fdr->init, &fdr->var, fout);
    fprintf(fout, "Goal:");
    pddlFDRPartStatePrettyPrint(&fdr->goal, &fdr->var, fout);
}
