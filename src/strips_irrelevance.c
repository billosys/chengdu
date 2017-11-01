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

#include <boruvka/alloc.h>

#include "pddl/strips.h"
#include "err.h"
#include "assert.h"

struct fact_info {
    int init; /*!< True if the fact is a part of the initial state */
    int goal; /*!< True if the fact is in the goal */

    bor_iset_t pre; /*!< Operators where the fact appears in its pre */
    bor_iset_t add_eff; /*!< Similarly for add effects */
    bor_iset_t del_eff; /*!< And for the del effects */
};
typedef struct fact_info fact_info_t;

struct prob_info {
    int fact_size;
    int op_size;
    fact_info_t *fact;
    int *fact_irrelevant;
    int fact_irrelevant_size;
    int *op_irrelevant;
    int op_irrelevant_size;
};
typedef struct prob_info prob_info_t;

static void probInfoInit(prob_info_t *prob, pddl_strips_t *strips)
{
    int fact_id;

    bzero(prob, sizeof(*prob));
    prob->fact_size = strips->fact.fact_size;
    prob->op_size = strips->op.op_size;

    prob->fact = BOR_CALLOC_ARR(fact_info_t, prob->fact_size);
    prob->fact_irrelevant = BOR_CALLOC_ARR(int, prob->fact_size);
    prob->fact_irrelevant_size = 0;
    prob->op_irrelevant = BOR_CALLOC_ARR(int, prob->op_size);
    prob->op_irrelevant_size = 0;

    BOR_ISET_FOR_EACH(&strips->init, fact_id)
        prob->fact[fact_id].init = 1;
    BOR_ISET_FOR_EACH(&strips->goal, fact_id)
        prob->fact[fact_id].goal = 1;

    for (int opi = 0; opi < prob->op_size; ++opi){
        const pddl_strips_op_t *op = strips->op.op[opi];
        BOR_ISET_FOR_EACH(&op->pre, fact_id)
            borISetAdd(&prob->fact[fact_id].pre, opi);
        BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
            borISetAdd(&prob->fact[fact_id].del_eff, opi);
        BOR_ISET_FOR_EACH(&op->add_eff, fact_id)
            borISetAdd(&prob->fact[fact_id].add_eff, opi);

        for (int cei = 0; cei < op->cond_eff_size; ++cei){
            const pddl_strips_op_cond_eff_t *ce = op->cond_eff + cei;
            BOR_ISET_FOR_EACH(&ce->pre, fact_id)
                borISetAdd(&prob->fact[fact_id].pre, opi);
            BOR_ISET_FOR_EACH(&ce->del_eff, fact_id)
                borISetAdd(&prob->fact[fact_id].del_eff, opi);
            BOR_ISET_FOR_EACH(&ce->add_eff, fact_id)
                borISetAdd(&prob->fact[fact_id].add_eff, opi);
        }
    }
}

static void probInfoFree(prob_info_t *prob)
{
    for (int i = 0; i < prob->fact_size; ++i){
        borISetFree(&prob->fact[i].pre);
        borISetFree(&prob->fact[i].add_eff);
        borISetFree(&prob->fact[i].del_eff);
    }
    if (prob->fact != NULL)
        BOR_FREE(prob->fact);
    if (prob->fact_irrelevant != NULL)
        BOR_FREE(prob->fact_irrelevant);
    if (prob->op_irrelevant != NULL)
        BOR_FREE(prob->op_irrelevant);
}

static void makeOpIrrelevant(pddl_strips_t *strips, prob_info_t *prob,
                             int op_id)
{
    const pddl_strips_op_t *op = strips->op.op[op_id];
    int fact_id;

    if (prob->op_irrelevant[op_id])
        return;

    prob->op_irrelevant[op_id] = 1;
    ++prob->op_irrelevant_size;
    BOR_ISET_FOR_EACH(&op->pre, fact_id)
        borISetRm(&prob->fact[fact_id].pre, op_id);
    BOR_ISET_FOR_EACH(&op->add_eff, fact_id)
        borISetRm(&prob->fact[fact_id].add_eff, op_id);
    BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
        borISetRm(&prob->fact[fact_id].del_eff, op_id);

    for (int cei = 0; cei < op->cond_eff_size; ++cei){
        const pddl_strips_op_cond_eff_t *ce = op->cond_eff + cei;
        BOR_ISET_FOR_EACH(&ce->pre, fact_id)
            borISetRm(&prob->fact[fact_id].pre, op_id);
        BOR_ISET_FOR_EACH(&ce->add_eff, fact_id)
            borISetRm(&prob->fact[fact_id].add_eff, op_id);
        BOR_ISET_FOR_EACH(&ce->del_eff, fact_id)
            borISetRm(&prob->fact[fact_id].del_eff, op_id);
    }
}

static void makeFactIrrelevant(pddl_strips_t *strips, prob_info_t *prob,
                               int fact_id)
{
    fact_info_t *fact = prob->fact + fact_id;
    int op_id;

    if (prob->fact_irrelevant[fact_id])
        return;

    prob->fact_irrelevant[fact_id] = 1;
    ++prob->fact_irrelevant_size;

    BOR_ISET_FOR_EACH(&fact->pre, op_id)
        pddlStripsOpRemoveFact(strips->op.op[op_id], fact_id);
    borISetEmpty(&fact->pre);

    BOR_ISET_FOR_EACH(&fact->add_eff, op_id)
        pddlStripsOpRemoveFact(strips->op.op[op_id], fact_id);
    borISetEmpty(&fact->add_eff);

    BOR_ISET_FOR_EACH(&fact->del_eff, op_id)
        pddlStripsOpRemoveFact(strips->op.op[op_id], fact_id);
    borISetEmpty(&fact->del_eff);

    if (fact->init)
        borISetRm(&strips->init, fact_id);
    if (fact->goal)
        borISetRm(&strips->goal, fact_id);
    fact->init = fact->goal = 0;
}

/**
 * Although during grounding the facts created from the static
 * predicates were already removed, there still can be facts that are
 * static individually, i.e., they appear only in the preconditions of
 * operators -- they are not manipulated in any way --, but their
 * predicates are not static, because they appear in effects.
 * If such a static fact appears in the initial state, then it is true
 * troughout the whole state space, therefore it can be safely removed
 * from all operators, init and goal.
 */
static int factStatic(pddl_strips_t *strips, prob_info_t *prob, int fact_id)
{
    if (prob->fact_irrelevant[fact_id])
        return 0;

    const fact_info_t *fact = prob->fact + fact_id;
    if (fact->add_eff.size == 0 && fact->del_eff.size == 0 && fact->init){
        makeFactIrrelevant(strips, prob, fact_id);
        return 1;
    }

    return 0;
}

static void backwardIrrelevanceEnqueue(const pddl_strips_t *strips,
                                       prob_info_t *prob, int op_id,
                                       int *relevant, int *op_relevant,
                                       int *queue, int *queue_size)
{
    const pddl_strips_op_t *op = strips->op.op[op_id];
    int next;

    if (prob->op_irrelevant[op_id] || op_relevant[op_id])
        return;
    op_relevant[op_id] = 1;

    BOR_ISET_FOR_EACH(&op->pre, next){
        if (prob->fact_irrelevant[next] || relevant[next])
            continue;
        relevant[next] = 1;
        queue[(*queue_size)++] = next;
    }

    for (int cei = 0; cei < op->cond_eff_size; ++cei){
        const pddl_strips_op_cond_eff_t *ce = op->cond_eff + cei;
        BOR_ISET_FOR_EACH(&ce->pre, next){
            if (prob->fact_irrelevant[next] || relevant[next])
                continue;
            relevant[next] = 1;
            queue[(*queue_size)++] = next;
        }
    }
}

static int backwardIrrelevance(pddl_strips_t *strips, prob_info_t *prob,
                               const pddl_strips_prune_config_t *cfg)
{
    int queue_size, *queue, *relevant, *op_relevant;
    int fact_id, op_id, ret = 0;

    queue = BOR_CALLOC_ARR(int, prob->fact_size);
    relevant = BOR_CALLOC_ARR(int, prob->fact_size);
    op_relevant = BOR_CALLOC_ARR(int, prob->op_size);

    // Initialize queue with the goal
    queue_size = 0;
    BOR_ISET_FOR_EACH(&strips->goal, fact_id){
        if (!prob->fact_irrelevant[fact_id]){
            queue[queue_size++] = fact_id;
            relevant[fact_id] = 1;
        }
    }

    while (queue_size > 0){
        fact_id = queue[--queue_size];
        const fact_info_t *fact = prob->fact + fact_id;
        BOR_ISET_FOR_EACH(&fact->add_eff, op_id){
            backwardIrrelevanceEnqueue(strips, prob, op_id, relevant,
                                       op_relevant, queue, &queue_size);
        }

        if (cfg->irrelevance_del_eff || strips->has_cond_eff){
            BOR_ISET_FOR_EACH(&fact->del_eff, op_id){
                backwardIrrelevanceEnqueue(strips, prob, op_id, relevant,
                                           op_relevant, queue, &queue_size);
            }
        }
    }

    // Operators that were not reached are irrelevant
    for (int op_id = 0; op_id < prob->op_size; ++op_id){
        if (!op_relevant[op_id] && !prob->op_irrelevant[op_id]){
            makeOpIrrelevant(strips, prob, op_id);
            ret = 1;
        }
    }

    // The unreached facts are irrelevant
    for (int fact_id = 0; fact_id < prob->fact_size; ++fact_id){
        if (!relevant[fact_id] && !prob->fact_irrelevant[fact_id]){
            makeFactIrrelevant(strips, prob, fact_id);
            ret = 1;
        }
    }

    BOR_FREE(op_relevant);
    BOR_FREE(relevant);
    BOR_FREE(queue);

    return ret;
}

static void remapInitGoal(pddl_strips_t *strips, const int *remap)
{
    borISetRemap(&strips->init, remap);
    borISetRemap(&strips->goal, remap);
}

int _pddlStripsPruneIrrelevant(pddl_strips_t *strips,
                               const pddl_strips_prune_config_t *cfg)
{
    prob_info_t pi;
    int ret, static_facts = 0;

    probInfoInit(&pi, strips);

    if (cfg->irrelevance)
        backwardIrrelevance(strips, &pi, cfg);
    if (cfg->static_facts){
        for (int fact_id = 0; fact_id < pi.fact_size; ++fact_id)
            static_facts += factStatic(strips, &pi, fact_id);
    }

    INFO("O: %d, F: %d :: static facts: %d.",
         strips->op.op_size, strips->fact.fact_size, static_facts);

    if (pi.op_irrelevant_size > 0)
        pddlStripsOpsDelOps(&strips->op, pi.op_irrelevant);

    if (pi.fact_irrelevant_size > 0){
        int *fact_remap = BOR_ALLOC_ARR(int, pi.fact_size);
        pddlFactsDelIrrelevantFacts(&strips->fact, pi.fact_irrelevant,
                                    fact_remap);
        pddlStripsOpsRemapFacts(&strips->op, fact_remap);
        remapInitGoal(strips, fact_remap);
        BOR_FREE(fact_remap);
    }

    ret = pi.fact_irrelevant_size + pi.op_irrelevant_size;

    INFO("O: %d, F: %d :: irrelevance analysis"
         " (irrelevant facts: %d, ops: %d).",
         strips->op.op_size, strips->fact.fact_size,
         pi.fact_irrelevant_size, pi.op_irrelevant_size);

    probInfoFree(&pi);
    return ret;
}
