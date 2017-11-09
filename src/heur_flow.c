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

#include "pddl/heur.h"
#include "err.h"
#include "assert.h"


#define BOUND_INF 1E30
#define ROUND_EPS 1E-6

struct fact {
    int is_goal;            /*!< True if the fact is in goal */
    int is_init;            /*!< True if the fact from the initial state */

    bor_iset_t mgroup; /*!< Mutex groups that contain this fact */
    bor_iset_t produce; /*!< Ops that produce this fact */
    bor_iset_t consume; /*!< Ops that consume this fact */

    double lower_bound; /*!< Lower bound on the constraint */
    double upper_bound; /*!< Upper bound */
    int is_eq_constr;   /*!< True if the constraint is equality */
};
typedef struct fact fact_t;

static fact_t *loadFacts(const pddl_strips_t *strips,
                         const bor_iset_t *init,
                         const bor_iset_t *goal)
{
    fact_t *fact = BOR_CALLOC_ARR(fact_t, strips->fact.fact_size);
    int fact_id;

    BOR_ISET_FOR_EACH(init, fact_id)
        fact[fact_id].is_init = 1;
    BOR_ISET_FOR_EACH(goal, fact_id)
        fact[fact_id].is_goal = 1;

    for (int i = 0; i < strips->mgroup.mgroup_size; ++i){
        const pddl_mgroup_t *mg = strips->mgroup.mgroup + i;
        BOR_ISET_FOR_EACH(&mg->fact, fact_id)
            borISetAdd(&fact[fact_id].mgroup, i);
    }

    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        ASSERT(borISetIntersectionSize(&op->pre, &op->del_eff)
                    == borISetSize(&op->del_eff));

        BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
            borISetAdd(&fact[fact_id].consume, i);

        BOR_ISET_FOR_EACH(&op->add_eff, fact_id){
            borISetAdd(&fact[fact_id].produce, i);
            ASSERT(pddlMutexesIsMutexWithFact(&strips->mutex, fact_id,
                        &op->pre));
        }
    }

    for (int i = 0; i < strips->fact.fact_size; ++i){
        // First set lower bounds
        if (fact[i].is_goal){
            if (fact[i].is_init){
                fact[i].lower_bound = 0.;
            }else{
                fact[i].lower_bound = 1.;
            }
        }else{
            if (fact[i].is_init){
                fact[i].lower_bound = -1.;
            }else{
                fact[i].lower_bound = 0.;
            }
        }

        // And now the upper bounds
        if (!fact[i].is_goal
                && !pddlMutexesIsMutexWithFact(&strips->mutex, i, goal)){
            fact[i].upper_bound = fact[i].lower_bound + 1;
        }else{
            fact[i].upper_bound = fact[i].lower_bound;
            fact[i].is_eq_constr = 1;
        }
    }

    return fact;
}

static void freeFacts(fact_t *fact, int fact_size)
{
    for (int i = 0; i < fact_size; ++i){
        borISetFree(&fact[i].mgroup);
        borISetFree(&fact[i].produce);
        borISetFree(&fact[i].consume);
    }
    BOR_FREE(fact);
}

static int numEqConstr(const fact_t *fact, int fact_size)
{
    int num = 0;
    for (int i = 0; i < fact_size; ++i)
        num += fact[i].is_eq_constr;
    return num;
}

static int roundOff(double z)
{
    int v = z;
    if (fabs(z - (double)v) > ROUND_EPS)
        return ceil(z);
    return v;
}

int pddlHeurFlow(const pddl_strips_t *strips_in,
                 const bor_iset_t *init,
                 const bor_iset_t *goal,
                 const pddl_landmarks_t *ldms)
{
    pddl_strips_t *strips;
    fact_t *fact;
    bor_lp_t *lp;
    unsigned lp_flags = 0;
    int num_rows, row, op_id, sret;
    double val, *obj;
    int hval;

    strips = pddlStripsClone(strips_in);
    pddlStripsCompleteMGroups(strips);
    if (pddlStripsMakeExactlyOneMGroups(strips) != 0){
        pddlStripsDel(strips);
        TRACE_RET(-1);
    }

    fact = loadFacts(strips, init, goal);
    num_rows = numEqConstr(fact, strips->fact.fact_size);
    num_rows += (strips->fact.fact_size - num_rows) * 2;
    if (ldms != NULL)
       num_rows += ldms->ldm_size;

    lp_flags  = BOR_LP_MIN;
    lp_flags |= BOR_LP_NUM_THREADS(1);
    lp = borLPNew(num_rows, strips->op.op_size, lp_flags);

    for (int i = 0; i < strips->op.op_size; ++i){
        borLPSetObj(lp, i, strips->op.op[i]->cost);
        borLPSetVarRange(lp, i, 0., BOUND_INF);
    }

    row = 0;
    for (int fact_id = 0; fact_id < strips->fact.fact_size; ++fact_id){
        BOR_ISET_FOR_EACH(&fact[fact_id].consume, op_id){
            borLPSetCoef(lp, row, op_id, -1.);
            if (!fact[fact_id].is_eq_constr)
                borLPSetCoef(lp, row + 1, op_id, -1.);
        }

        BOR_ISET_FOR_EACH(&fact[fact_id].produce, op_id){
            borLPSetCoef(lp, row, op_id, 1.);
            if (!fact[fact_id].is_eq_constr)
                borLPSetCoef(lp, row + 1, op_id, 1.);
        }

        if (fact[fact_id].is_eq_constr){
            borLPSetRHS(lp, row, fact[fact_id].lower_bound, 'E');
            ++row;
        }else{
            borLPSetRHS(lp, row, fact[fact_id].lower_bound, 'G');
            borLPSetRHS(lp, row + 1, fact[fact_id].upper_bound, 'L');
            row += 2;
        }
    }

    for (int ldmi = 0; ldms != NULL && ldmi < ldms->ldm_size; ++ldmi){
        const pddl_landmark_t *ldm = ldms->ldm[ldmi];
        BOR_ISET_FOR_EACH(&ldm->op, op_id)
            borLPSetCoef(lp, row, op_id, 1.);
        borLPSetRHS(lp, row, 1., 'G');
        ++row;
    }

    ASSERT(row == num_rows);

    obj = BOR_ALLOC_ARR(double, strips->op.op_size);
    sret = borLPSolve(lp, &val, obj);
    ASSERT(sret == 0);
    hval = roundOff(val);

    BOR_FREE(obj);
    borLPDel(lp);
    freeFacts(fact, strips->fact.fact_size);

    pddlStripsDel(strips);
    return hval;
}
