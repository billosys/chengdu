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
#include <boruvka/lp.h>
#include "pddl/mgroup.h"

int addCEConstrOp(bor_lp_t *lp, const pddl_strips_t *strips,
                  const pddl_strips_op_t *op, int cond_eff_id,
                  const bor_iset_t *opre,
                  const bor_iset_t *oadd_eff,
                  const bor_iset_t *odel_eff,
                  int rows)
{
    double rhs = 0.;
    char sense = 'L';
    bor_iset_t pre, add_eff, del_eff, predel;
    int fact;

    borISetInit(&pre);
    borISetInit(&add_eff);
    borISetInit(&del_eff);
    borISetInit(&predel);
    borISetUnion(&pre, opre);
    borISetUnion(&pre, &op->cond_eff[cond_eff_id].pre);
    borISetUnion(&add_eff, oadd_eff);
    borISetUnion(&add_eff, &op->cond_eff[cond_eff_id].add_eff);
    borISetUnion(&del_eff, odel_eff);
    borISetUnion(&del_eff, &op->cond_eff[cond_eff_id].del_eff);

    // make the combined operator well-formed
    borISetMinus(&del_eff, &add_eff);
    borISetMinus(&add_eff, &pre);

    // pre \cap del
    borISetUnion(&predel, &pre);
    borISetIntersect(&predel, &del_eff);

    borLPAddRows(lp, 1, &rhs, &sense);
    BOR_ISET_FOR_EACH(&add_eff, fact)
        borLPSetCoef(lp, rows, fact, 1.);
    BOR_ISET_FOR_EACH(&predel, fact)
        borLPSetCoef(lp, rows, fact, -1.);
    rows += 1;

    for (int i = cond_eff_id + 1; i < op->cond_eff_size; ++i){
        rows = addCEConstrOp(lp, strips, op, i, &pre, &add_eff, &del_eff, rows);
    }

    borISetFree(&pre);
    borISetFree(&add_eff);
    borISetFree(&del_eff);
    borISetFree(&predel);
    return rows;
}

int addCEConstr(bor_lp_t *lp, const pddl_strips_t *strips, int rows)
{
    for (int oi = 0; oi < strips->op.op_size; ++oi){
        const pddl_strips_op_t *op = strips->op.op[oi];
        for (int i = 0; i < op->cond_eff_size; ++i){
            rows = addCEConstrOp(lp, strips, op, i, &op->pre,
                                 &op->add_eff, &op->del_eff, rows);
        }
    }

    return rows;
}

pddl_mgroups_t *pddlMGroupFindFA(const pddl_strips_t *strips)
{
    pddl_mgroups_t *mgs;
    pddl_mgroup_t *mg;
    bor_lp_t *lp;
    unsigned lp_flags;
    bor_iset_t predel, fa_mgroup;
    int rows, fact, has_ce = 0;
    double val, *obj;

    // TODO: Check on availability of LP-solver!
    lp_flags  = BOR_LP_DEFAULT;
    lp_flags |= BOR_LP_NUM_THREADS(1); // TODO: Parametrize
    lp_flags |= BOR_LP_MAX;
    rows = strips->op.op_size + 1;
    lp = borLPNew(rows, strips->fact.fact_size, lp_flags);

    // Set up coeficients in the objective function and set up binary
    // variables
    for (int i = 0; i < strips->fact.fact_size; ++i){
        borLPSetObj(lp, i, 1.);
        borLPSetVarBinary(lp, i);
    }

    // Initial state constraintf
    BOR_ISET_FOR_EACH(&strips->init, fact)
        borLPSetCoef(lp, 0, fact, 1.);
    borLPSetRHS(lp, 0, 1., 'L');

    // Operator constraints
    borISetInit(&predel);
    for (int oi = 0; oi < strips->op.op_size; ++oi){
        const pddl_strips_op_t *op = strips->op.op[oi];
        BOR_ISET_FOR_EACH(&op->add_eff, fact)
            borLPSetCoef(lp, oi + 1, fact, 1.);

        borISetEmpty(&predel);
        borISetUnion(&predel, &op->pre);
        borISetIntersect(&predel, &op->del_eff);
        BOR_ISET_FOR_EACH(&predel, fact)
            borLPSetCoef(lp, oi + 1, fact, -1.);
        borLPSetRHS(lp, oi + 1, 0., 'L');

        if (op->cond_eff_size > 0)
            has_ce = 1;
    }
    borISetFree(&predel);

    // Add constraints for conditional effects
    if (has_ce)
        rows = addCEConstr(lp, strips, rows);

    mgs = pddlMGroupsNew();
    borISetInit(&fa_mgroup);
    obj = BOR_ALLOC_ARR(double, strips->fact.fact_size);
    while (borLPSolve(lp, &val, obj) == 0 && val > 1.5){
        double rhs = 1.;
        char sense = 'G';
        borLPAddRows(lp, 1, &rhs, &sense);
        borISetEmpty(&fa_mgroup);
        for (int i = 0; i < strips->fact.fact_size; ++i){
            if (obj[i] < 0.5){
                borLPSetCoef(lp, rows, i, 1.);
            }else{
                borISetAdd(&fa_mgroup, i);
            }
        }
        mg = pddlMGroupsAdd(mgs, &fa_mgroup);
        mg->is_fa = 1;
        ++rows;
    }
    BOR_FREE(obj);
    borISetFree(&fa_mgroup);

    borLPDel(lp);

    return mgs;
}
