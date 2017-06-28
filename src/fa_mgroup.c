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

pddl_mgroups_t *pddlMGroupFindFA(const pddl_strips_t *strips)
{
    pddl_mgroups_t *mgs;
    pddl_mgroup_t *mg;
    bor_lp_t *lp;
    unsigned lp_flags;
    const pddl_strips_op_t *op;
    bor_iset_t predel, fa_mgroup;
    int rows, fact;
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
        op = strips->op.op[oi];
        BOR_ISET_FOR_EACH(&op->add_eff, fact)
            borLPSetCoef(lp, oi + 1, fact, 1.);

        borISetEmpty(&predel);
        borISetUnion(&predel, &op->pre);
        borISetIntersect(&predel, &op->del_eff);
        BOR_ISET_FOR_EACH(&predel, fact)
            borLPSetCoef(lp, oi + 1, fact, -1.);
        borLPSetRHS(lp, oi + 1, 0., 'L');

        // TODO: Conditional effects
    }
    borISetFree(&predel);

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
