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

/** Implemented in strips_ground.c */
void _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl);
/** Prunes irrelevant facts and operators.
 *  Implemented in strips_irrelevance.c */
int _pddlStripsPruneIrrelevant(pddl_strips_t *strips);

static pddl_strips_t *stripsNew(const pddl_t *pddl)
{
    pddl_strips_t *strips;

    strips = BOR_ALLOC(pddl_strips_t);
    bzero(strips, sizeof(*strips));
    strips->pddl = pddl;
    pddlFactsInit(&strips->fact);
    pddlStripsOpsInit(&strips->op);
    borISetInit(&strips->init);
    borISetInit(&strips->goal);
    return strips;
}

pddl_strips_t *pddlStripsGround(const pddl_t *pddl, unsigned flags)
{
    pddl_strips_t *strips = stripsNew(pddl);

    _pddlStripsGround(strips, pddl);
    _pddlStripsPruneIrrelevant(strips);

    // TODO: remove static facts
    // TODO: remove identical operators (don't forget to keep the one with
    // the minimal cost)
    // TODO: causal graph
    // TODO: pruning
    // TODO: is goal reachable?
    // TODO: Irrelevance == reachability from goal and from init
    // TODO: Operators without add effects cannot be part of the stricly
    //       optimal plan.

    return strips;
}

void pddlStripsDel(pddl_strips_t *strips)
{
    pddlFactsFree(&strips->fact);
    pddlStripsOpsFree(&strips->op);
    borISetFree(&strips->init);
    borISetFree(&strips->goal);
    BOR_FREE(strips);
}

pddl_strips_t *pddlStripsDual(const pddl_strips_t *strips)
{
    pddl_strips_t *dual = stripsNew(strips->pddl);
    pddl_strips_op_t op;

    pddlFactsCopy(&dual->fact, &strips->fact);

    // Construct initial state and goal specification
    for (int i = 0; i < dual->fact.fact_size; ++i){
        borISetAdd(&dual->init, i);
        borISetAdd(&dual->goal, i);
    }
    borISetMinus(&dual->init, &strips->goal);
    borISetMinus(&dual->goal, &strips->init);

    // Copy dual operators
    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *sop = strips->op.op[i];
        pddlStripsOpInit(&op);
        pddlStripsOpCopyDual(&op, sop);
        pddlStripsOpsAdd(&dual->op, &op);
        pddlStripsOpFree(&op);
    }

    dual->goal_is_unreachable = strips->goal_is_unreachable;
    dual->has_cond_eff = strips->has_cond_eff;

    return dual;
}

static void expandCondEff(pddl_strips_t *dst,
                          const pddl_strips_op_t *base_op,
                          const pddl_strips_op_t *ce_op,
                          int cond_eff_id)
{
    const pddl_strips_op_cond_eff_t *ce;
    pddl_strips_op_t op;

    pddlStripsOpInit(&op);
    pddlStripsOpCopy(&op, base_op);

    ce = ce_op->cond_eff + cond_eff_id;
    borISetUnion(&op.pre, &ce->pre);
    borISetUnion(&op.del_eff, &ce->del_eff);
    borISetUnion(&op.add_eff, &ce->add_eff);
    pddlStripsOpNormalize(&op);
    pddlStripsOpsAdd(&dst->op, &op);

    for (int i = cond_eff_id + 1; i < ce_op->cond_eff_size; ++i)
        expandCondEff(dst, &op, ce_op, i);

    pddlStripsOpFree(&op);
}

pddl_strips_t *pddlStripsCompileOutCondEff(const pddl_strips_t *strips)
{
    pddl_strips_t *s = stripsNew(strips->pddl);
    pddl_strips_op_t op;

    pddlFactsCopy(&s->fact, &strips->fact);
    borISetUnion(&s->init, &strips->init);
    borISetUnion(&s->goal, &strips->goal);

    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *sop = strips->op.op[i];
        pddlStripsOpInit(&op);
        pddlStripsOpCopyWithoutCondEff(&op, sop);
        pddlStripsOpNormalize(&op);
        pddlStripsOpsAdd(&s->op, &op);
        for (int ce = 0; ce < sop->cond_eff_size; ++ce)
            expandCondEff(s, &op, sop, ce);
        pddlStripsOpFree(&op);
    }

    s->goal_is_unreachable = strips->goal_is_unreachable;

    return s;
}

pddl_strips_t *pddlStripsCompileOutCondEffRelaxed(const pddl_strips_t *strips)
{
    pddl_strips_t *s = stripsNew(strips->pddl);
    pddl_strips_op_t op;

    pddlFactsCopy(&s->fact, &strips->fact);
    borISetUnion(&s->init, &strips->init);
    borISetUnion(&s->goal, &strips->goal);

    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *sop = strips->op.op[i];
        pddlStripsOpInit(&op);
        pddlStripsOpCopyWithoutCondEff(&op, sop);
        pddlStripsOpNormalize(&op);
        pddlStripsOpsAdd(&s->op, &op);
        pddlStripsOpFree(&op);
        for (int cei = 0; cei < sop->cond_eff_size; ++cei){
            const pddl_strips_op_cond_eff_t *ce = sop->cond_eff + cei;
            pddlStripsOpInit(&op);
            pddlStripsOpCopyWithoutCondEff(&op, sop);
            borISetUnion(&op.pre, &ce->pre);
            borISetUnion(&op.del_eff, &ce->del_eff);
            borISetUnion(&op.add_eff, &ce->add_eff);
            pddlStripsOpNormalize(&op);
            pddlStripsOpsAdd(&s->op, &op);
            pddlStripsOpFree(&op);
        }
    }

    s->goal_is_unreachable = strips->goal_is_unreachable;

    return s;
}

void pddlStripsDump(const pddl_strips_t *strips, FILE *fout)
{
    fprintf(fout, "Fact[%d]:\n", strips->fact.fact_size);
    pddlFactsPrintSorted(strips->pddl, &strips->fact, fout);

    fprintf(fout, "Op[%d]:\n", strips->op.op_size);
    pddlStripsOpsPrint(strips->pddl, &strips->fact, &strips->op, fout);

    fprintf(fout, "Init State: ");
    pddlFactIdSetPrettyPrint(strips->pddl, &strips->fact, &strips->init, fout);

    fprintf(fout, "Goal: ");
    pddlFactIdSetPrettyPrint(strips->pddl, &strips->fact, &strips->goal, fout);
    if (strips->goal_is_unreachable)
        fprintf(fout, "Goal is unreachable\n");
    if (strips->has_cond_eff)
        fprintf(fout, "Has conditional effects\n");
}


static void removeIrrelevantFacts(pddl_strips_t *strips)
{
    bor_iset_t *graph;
    int *queue, queue_size;
    int *relevant;
    int from, to, fact_id;

    // Construct support graph with inverse edges, i.e., facts are nodes
    // and edges lead from add effects to preconditions.
    graph = BOR_CALLOC_ARR(bor_iset_t, strips->fact.fact_size);
    for (int opi = 0; opi < strips->op.op_size; ++opi){
        const pddl_strips_op_t *op = strips->op.op[opi];
        BOR_ISET_FOR_EACH(&op->add_eff, from){
            bor_iset_t *f = graph + from;
            BOR_ISET_FOR_EACH(&op->pre, to)
                borISetAdd(f, to);
        }
    }

    relevant = BOR_CALLOC_ARR(int, strips->fact.fact_size);

    // Initilize queue with the goal
    queue_size = 0;
    queue = BOR_ALLOC_ARR(int, strips->fact.fact_size);
    BOR_ISET_FOR_EACH(&strips->goal, fact_id){
        queue[queue_size++] = fact_id;
        relevant[fact_id] = 2;
    }

    while (queue_size > 0){
        fact_id = queue[--queue_size];
        relevant[fact_id] = 1;
        BOR_ISET_FOR_EACH(graph + fact_id, to){
            if (relevant[to] == 0){
                relevant[to] = 2;
                queue[queue_size++] = to;
            }
        }
    }
    BOR_FREE(queue);

    for (int i = 0; i < strips->fact.fact_size; ++i){
        ASSERT(relevant[i] != 2);
        if (!relevant[i]){
            for (int j = 0; j < strips->op.op_size; ++j){
                int f, x = 0;
                /*
                BOR_ISET_FOR_EACH(&strips->op.op[j]->add_eff, f){
                    if (f == i){
                        printf(" A::");
                        pddlFactPrint(strips->pddl, strips->fact.fact[i], stdout);
                        x = 1;
                        break;
                    }
                }
                */
                BOR_ISET_FOR_EACH(&strips->op.op[j]->del_eff, f){
                    if (f == i){
                        printf(" D::");
                        pddlFactPrint(strips->pddl, strips->fact.fact[i], stdout);
                        x = 1;
                        break;
                    }
                }
                if (x)
                    break;
            }
        }
    }
    printf("\n");
    BOR_FREE(relevant);

    for (int i = 0; i < strips->fact.fact_size; ++i)
        borISetFree(graph + i);
    BOR_FREE(graph);
}
