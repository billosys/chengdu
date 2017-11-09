/***
 * maplan
 * -------
 * Copyright (c)2017 Daniel Fiser <danfis@danfis.cz>,
 * AIC, Department of Computer Science,
 * Faculty of Electrical Engineering, Czech Technical University in Prague.
 * All rights reserved.
 *
 * This file is part of maplan.
 *
 * Distributed under the OSI-approved BSD License (the "License");
 * see accompanying file BDS-LICENSE for details or see
 * <http://www.opensource.org/licenses/bsd-license.php>.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the License for more information.
 */

#include <limits.h>
#include <boruvka/alloc.h>
#include <boruvka/apq.h>

#include "pddl/heur.h"
#include "err.h"
#include "assert.h"


#define CUT_UNDEF 0
#define CUT_INIT 1
#define CUT_GOAL 2

struct pddl_lm_cut_op {
    bor_iset_t pre;
    bor_iset_t eff;
    int op_cost;

    int cost;          /*!< Current cost of the operator */
    int unsat;         /*!< Number of unsatisfied preconditions */
    int supp;          /*!< Supporter fact (that maximizes h^max) */
    int supp_cost;     /*!< Cost of the supported -- needed for hMaxInc() */
    int cut_candidate; /*!< True if the operator is candidate for a cut */
};
typedef struct pddl_lm_cut_op pddl_lm_cut_op_t;

struct pddl_lm_cut_fact {
    bor_iset_t pre_op; /*!< Operators having this fact as its precond */
    bor_iset_t eff_op; /*!< Operators having this fact as its effect */
    int value;
    bor_apq_el_t pq; /*!< Connection to priority queue */
    int supp_cnt;    /*!< Number of operators that have this fact as
                          a supporter. */
    int cut_state; /*!< One of CUT_* */
};
typedef struct pddl_lm_cut_fact pddl_lm_cut_fact_t;

struct pddl_lm_cut {
    pddl_lm_cut_fact_t *fact;
    int fact_size;
    int fact_goal; /*!< ID of the artificial goal fact */
    int fact_nopre; /*!< ID of the artifical fact meaning "no precondition" */

    pddl_lm_cut_op_t *op;
    int op_size;
    int op_goal; /*!< ID of the artificial goal operator */

    bor_iset_t state; /*!< Current state from which heur is computed */
    bor_iset_t cut; /*!< Current cut */

    /** Auxiliary structures to avoid re-allocation */
    int *queue;
    int queue_size;
    bor_apq_t pq;
};
typedef struct pddl_lm_cut pddl_lm_cut_t;

#define FID(heur, f) ((f) - (heur)->fact)
#define FVALUE(fact) ((fact)->value)
#define FVALUE_INIT(fact) \
    do { (fact)->value = (fact)->pq.key = INT_MAX; } while(0)
#define FVALUE_IS_SET(fact) (FVALUE(fact) != INT_MAX)

_bor_inline void FPUSH(bor_apq_t *pq, int val, pddl_lm_cut_fact_t *fact)
{
    ASSERT(val != INT_MAX);
    if (fact->pq.key != INT_MAX){
        fact->value = val;
        borAPQUpdate(pq, val, &fact->pq);
    }else{
        (fact)->value = (val);
        borAPQPush(pq, val, &fact->pq);
    }
}

_bor_inline pddl_lm_cut_fact_t *FPOP(bor_apq_t *pq, int *key)
{
    bor_apq_el_t *el = borAPQPop(pq, key);
    pddl_lm_cut_fact_t *fact = bor_container_of(el, pddl_lm_cut_fact_t, pq);
    fact->pq.key = INT_MAX;
    return fact;
}

#define SET_OP_SUPP(h, op, fact_id) \
    do { \
        if ((op)->supp != -1) \
            F_UNSET_SUPP((h)->fact + (op)->supp); \
        (op)->supp = (fact_id); \
        (op)->supp_cost = (h)->fact[(fact_id)].value; \
        F_SET_SUPP((h)->fact + (fact_id)); \
    } while (0)

#define F_INIT_SUPP(fact) ((fact)->supp_cnt = 0)
#define F_SET_SUPP(fact) (++(fact)->supp_cnt)
#define F_UNSET_SUPP(fact) (--(fact)->supp_cnt)
#define F_IS_SUPP(fact) ((fact)->supp_cnt)

static void lmCutInit(pddl_lm_cut_t *lmcut, const pddl_strips_t *strips)
{
    bzero(lmcut, sizeof(*lmcut));

    lmcut->fact_size = strips->fact.fact_size + 2;
    lmcut->fact = BOR_CALLOC_ARR(pddl_lm_cut_fact_t, lmcut->fact_size);
    lmcut->fact_goal = lmcut->fact_size - 2;
    lmcut->fact_nopre = lmcut->fact_size - 1;

    lmcut->op_size = strips->op.op_size + 1;
    lmcut->op = BOR_CALLOC_ARR(pddl_lm_cut_op_t, lmcut->op_size);
    lmcut->op_goal = lmcut->op_size - 1;

    pddlStripsCrossRefFactsOps(strips, lmcut->fact,
                               sizeof(pddl_lm_cut_fact_t),
                               bor_offsetof(pddl_lm_cut_fact_t, pre_op),
                               bor_offsetof(pddl_lm_cut_fact_t, eff_op),
                               -1);
    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        borISetUnion(&lmcut->op[i].pre, &op->pre);
        borISetUnion(&lmcut->op[i].eff, &op->add_eff);
        lmcut->op[i].op_cost = op->cost;
        if (borISetSize(&op->pre) == 0)
            borISetAdd(&lmcut->fact[lmcut->fact_nopre].pre_op, i);
    }

    borISetAdd(&lmcut->fact[lmcut->fact_goal].eff_op, lmcut->op_goal);
    borISetAdd(&lmcut->op[lmcut->op_goal].eff, lmcut->fact_goal);
    lmcut->op[lmcut->op_goal].op_cost = 0;

    borISetInit(&lmcut->state);
    borISetInit(&lmcut->cut);

    lmcut->queue = BOR_ALLOC_ARR(int, lmcut->fact_size);
    borAPQInit(&lmcut->pq);
}

static void lmCutFree(pddl_lm_cut_t *lmcut)
{
    for (int i = 0; i < lmcut->fact_size; ++i){
        borISetFree(&lmcut->fact[i].pre_op);
        borISetFree(&lmcut->fact[i].eff_op);
    }
    if (lmcut->fact != NULL)
        BOR_FREE(lmcut->fact);

    for (int i = 0; i < lmcut->op_size; ++i){
        borISetFree(&lmcut->op[i].pre);
        borISetFree(&lmcut->op[i].eff);
    }
    if (lmcut->op != NULL)
        BOR_FREE(lmcut->op);

    borISetFree(&lmcut->state);
    borISetFree(&lmcut->cut);

    if (lmcut->queue != NULL)
        BOR_FREE(lmcut->queue);
    borAPQFree(&lmcut->pq);
}


static void initFacts(pddl_lm_cut_t *lm)
{
    for (int i = 0; i < lm->fact_size; ++i){
        FVALUE_INIT(lm->fact + i);
        F_INIT_SUPP(lm->fact + i);
    }
}

static void initOps(pddl_lm_cut_t *lm, int init_cost)
{
    for (int i = 0; i < lm->op_size; ++i){
        lm->op[i].unsat = borISetSize(&lm->op[i].pre);
        lm->op[i].supp = -1;
        lm->op[i].supp_cost = INT_MAX;
        if (init_cost)
            lm->op[i].cost = lm->op[i].op_cost;
        lm->op[i].cut_candidate = 0;
    }
}

static void setInitState(pddl_lm_cut_t *lm)
{
    int fact_id;

    BOR_ISET_FOR_EACH(&lm->state, fact_id)
        FPUSH(&lm->pq, 0, lm->fact + fact_id);
    FPUSH(&lm->pq, 0, lm->fact + lm->fact_nopre);
}

static void enqueueOpEffects(pddl_lm_cut_t *lm,
                             pddl_lm_cut_op_t *op, int fact_value)
{
    int value = op->cost + fact_value;
    int fact_id;

    BOR_ISET_FOR_EACH(&op->eff, fact_id){
        pddl_lm_cut_fact_t *fact = lm->fact + fact_id;
        if (FVALUE(fact) > value)
            FPUSH(&lm->pq, value, fact);
    }
}

static void hMaxFull(pddl_lm_cut_t *lmcut, int init_cost)
{
    pddl_lm_cut_fact_t *fact;
    int value, op_id;

    initFacts(lmcut);
    initOps(lmcut, init_cost);
    setInitState(lmcut);
    while (!borAPQIsEmpty(&lmcut->pq)){
        fact = FPOP(&lmcut->pq, &value);
        ASSERT(FVALUE(fact) == value);

        BOR_ISET_FOR_EACH(&fact->pre_op, op_id){
            pddl_lm_cut_op_t *op = lmcut->op + op_id;
            if (--op->unsat == 0){
                // Set as supporter the last fact that enabled this
                // operator (it must be one of those that have maximum
                // value
                SET_OP_SUPP(lmcut, op, fact - lmcut->fact);
                enqueueOpEffects(lmcut, op, value);
            }
        }
    }
}

static void updateSupp(pddl_lm_cut_t *lm, pddl_lm_cut_op_t *op)
{
    int fact_id, supp = -1, value = -1;

    BOR_ISET_FOR_EACH(&op->pre, fact_id){
        pddl_lm_cut_fact_t *fact = lm->fact + fact_id;
        if (FVALUE_IS_SET(fact) && FVALUE(fact) > value){
            value = FVALUE(fact);
            supp = fact_id;
        }
    }

    ASSERT(supp != -1);
    SET_OP_SUPP(lm, op, supp);
}

static void enqueueOpEffectsInc(pddl_lm_cut_t *lm, pddl_lm_cut_op_t *op,
                                int fact_value)
{
    int value = op->cost + fact_value;
    int fact_id;

    // Check all base effects
    BOR_ISET_FOR_EACH(&op->eff, fact_id){
        pddl_lm_cut_fact_t *fact = lm->fact + fact_id;
        if (FVALUE(fact) > value)
            FPUSH(&lm->pq, value, fact);
    }
}

static void hMaxIncUpdateOp(pddl_lm_cut_t *lm, pddl_lm_cut_op_t *op,
                            int fact_id, int fact_value)
{
    int old_supp_value;

    if (op->supp != fact_id || op->unsat > 0)
        return;

    old_supp_value = op->supp_cost;
    if (old_supp_value <= fact_value)
        return;

    updateSupp(lm, op);
    if (op->supp_cost != old_supp_value){
        ASSERT(op->supp_cost < old_supp_value);
        enqueueOpEffectsInc(lm, op, op->supp_cost);
    }
}

static void hMaxInc(pddl_lm_cut_t *lm, const bor_iset_t *cut)
{
    pddl_lm_cut_fact_t *fact;
    pddl_lm_cut_op_t *op;
    int op_id, fact_id, fact_value;

    for (op_id = 0; op_id < lm->op_size; ++op_id)
        lm->op[op_id].cut_candidate = 0;

    BOR_ISET_FOR_EACH(cut, op_id){
        op = lm->op + op_id;
        enqueueOpEffectsInc(lm, op, op->supp_cost);
    }

    while (!borAPQIsEmpty(&lm->pq)){
        fact = FPOP(&lm->pq, &fact_value);
        fact_id = FID(lm, fact);

        BOR_ISET_FOR_EACH(&fact->pre_op, op_id){
            op = lm->op + op_id;
            hMaxIncUpdateOp(lm, op, fact_id, fact_value);
        }
    }
}



/** Mark facts connected with the goal with zero cost paths */
static void markGoalZone(pddl_lm_cut_t *lm)
{
    pddl_lm_cut_fact_t *fact;
    pddl_lm_cut_op_t *op;
    int fact_id, op_id;

    lm->queue_size = 1;
    lm->queue[0] = lm->fact_goal;
    lm->fact[lm->fact_goal].cut_state = CUT_GOAL;
    while (lm->queue_size > 0){
        fact_id = lm->queue[--lm->queue_size];
        fact = lm->fact + fact_id;
        BOR_ISET_FOR_EACH(&fact->eff_op, op_id){
            op = lm->op + op_id;
            if (op->supp >= 0 && lm->fact[op->supp].cut_state == CUT_UNDEF){
                if (op->cost == 0){
                    lm->queue[lm->queue_size++] = op->supp;
                    lm->fact[op->supp].cut_state = CUT_GOAL;
                }else{
                    op->cut_candidate = 1;
                }
            }
        }
    }
}

/** Finds cut (and fills h->cut) and returns cost of the cut.
 *  Requires marked goal zone. */
static int findCut(pddl_lm_cut_t *lm)
{
    pddl_lm_cut_op_t *op;
    int fact_id, op_id, next;
    int min_cost = INT_MAX;

    lm->queue_size = 0;
    BOR_ISET_FOR_EACH(&lm->state, fact_id){
        if (lm->fact[fact_id].cut_state == CUT_UNDEF){
            lm->queue[lm->queue_size++] = fact_id;
            lm->fact[fact_id].cut_state = CUT_INIT;
        }
    }

    borISetEmpty(&lm->cut);
    while (lm->queue_size > 0){
        fact_id = lm->queue[--lm->queue_size];
        BOR_ISET_FOR_EACH(&lm->fact[fact_id].pre_op, op_id){
            op = lm->op + op_id;
            if (op->supp != fact_id)
                continue;
            if (op->cut_candidate){
                borISetAdd(&lm->cut, op_id);
                min_cost = BOR_MIN(min_cost, op->cost);
                continue;
            }

            BOR_ISET_FOR_EACH(&op->eff, next){
                if (lm->fact[next].cut_state == CUT_UNDEF){
                    if (F_IS_SUPP(lm->fact + next)){
                        lm->fact[next].cut_state = CUT_INIT;
                        lm->queue[lm->queue_size++] = next;
                    }
                }
            }
        }
    }

    if (borISetSize(&lm->cut) == 0){
        FATAL2("Empty cut!");
    }else if (min_cost <= 0){
        FATAL("Invalid cut cost: %d!\n", min_cost);
    }

    return min_cost;
}


/** Decrease cost of the operators in the cut */
static void applyCutCost(pddl_lm_cut_t *lm, int min_cost)
{
    int op_id;
    BOR_ISET_FOR_EACH(&lm->cut, op_id)
        lm->op[op_id].cost -= min_cost;
}

/** Perform cut */
static int cut(pddl_lm_cut_t *lm)
{
    int cost;

    for (int i = 0; i < lm->fact_size; ++i)
        lm->fact[i].cut_state = CUT_UNDEF;
    markGoalZone(lm);
    cost = findCut(lm);
    applyCutCost(lm, cost);
    return cost;
}

static void setInitGoal(pddl_lm_cut_t *lmcut,
                        const bor_iset_t *init, const bor_iset_t *goal)
{
    int fact_id;

    borISetEmpty(&lmcut->state);
    borISetUnion(&lmcut->state, init);

    BOR_ISET_FOR_EACH(&lmcut->op[lmcut->op_goal].pre, fact_id)
        borISetRm(&lmcut->fact[fact_id].pre_op, lmcut->op_goal);

    borISetEmpty(&lmcut->op[lmcut->op_goal].pre);
    borISetUnion(&lmcut->op[lmcut->op_goal].pre, goal);
    BOR_ISET_FOR_EACH(goal, fact_id)
        borISetAdd(&lmcut->fact[fact_id].pre_op, lmcut->op_goal);
}

static int lmCut(pddl_lm_cut_t *lmcut,
                 const bor_iset_t *init, const bor_iset_t *goal,
                 pddl_landmarks_t *ldms)
{
    int heur = 0;

    setInitGoal(lmcut, init, goal);
    hMaxFull(lmcut, 1);
    if (!FVALUE_IS_SET(lmcut->fact + lmcut->fact_goal))
        return -1;

    while (FVALUE(lmcut->fact + lmcut->fact_goal) > 0){
        heur += cut(lmcut);
        if (ldms != NULL)
            pddlLandmarksAdd(ldms, &lmcut->cut);

        //hMaxFull(lmcut, 0);
        hMaxInc(lmcut, &lmcut->cut);
    }

    return heur;
}

int pddlHeurLMCut(const pddl_strips_t *strips,
                  const bor_iset_t *init,
                  const bor_iset_t *goal,
                  pddl_landmarks_t *ldms)
{
    pddl_lm_cut_t lmcut;
    int hval;

    lmCutInit(&lmcut, strips);
    hval = lmCut(&lmcut, init, goal, ldms);
    lmCutFree(&lmcut);
    return hval;
}
