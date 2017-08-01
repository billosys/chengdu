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

struct fact_info {
    int pre;
    int add_eff;
    int del_eff;
    int init;
    int goal;
};
typedef struct fact_info fact_info_t;

/** Implemented in strips_ground.c */
void _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl);

static void getFactInfo(const pddl_strips_t *strips, fact_info_t *fi);
static void removeStaticFacts(pddl_strips_t *strips);
static void removeFacts(pddl_strips_t *strips, const bor_iset_t *rm);
static void removeOpsWithUnreachablePre(pddl_strips_t *strips,
                                        const bor_iset_t *unreachable_facts);

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
    removeStaticFacts(strips);

    // TODO: remove static facts
    // TODO: remove identical operators (don't forget to keep the one with
    // the minimal cost)
    // TODO: causal graph
    // TODO: pruning
    // TODO: Relevance analysis
    // TODO: is goal reachable?
    // TODO: Compile away conditional effects if set in flags

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


static void getFactInfo(const pddl_strips_t *strips, fact_info_t *fi)
{
    const pddl_strips_op_t *op;
    const pddl_strips_op_cond_eff_t *ce;
    int fact_id;

    bzero(fi, sizeof(*fi) * strips->fact.fact_size);

    BOR_ISET_FOR_EACH(&strips->init, fact_id)
        fi[fact_id].init = 1;
    BOR_ISET_FOR_EACH(&strips->goal, fact_id)
        fi[fact_id].goal = 1;

    for (int i = 0; i < strips->op.op_size; ++i){
        op = strips->op.op[i];
        BOR_ISET_FOR_EACH(&op->pre, fact_id)
            fi[fact_id].pre = 1;
        BOR_ISET_FOR_EACH(&op->add_eff, fact_id)
            fi[fact_id].add_eff = 1;
        BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
            fi[fact_id].del_eff = 1;

        for (int cei = 0; cei < op->cond_eff_size; ++cei){
            ce = op->cond_eff + cei;
            BOR_ISET_FOR_EACH(&ce->pre, fact_id)
                fi[fact_id].pre = 1;
            BOR_ISET_FOR_EACH(&ce->add_eff, fact_id)
                fi[fact_id].add_eff = 1;
            BOR_ISET_FOR_EACH(&ce->del_eff, fact_id)
                fi[fact_id].del_eff = 1;
        }
    }
}

static void removeStaticFacts(pddl_strips_t *strips)
{
    const pddl_strips_op_t *op;
    int fact_id;
    int *is_not_static;
    bor_iset_t static_facts;
    bor_iset_t static_unreachable_facts;

    // Find static facts
    is_not_static = BOR_CALLOC_ARR(int, strips->fact.fact_size);
    for (int i = 0; i < strips->op.op_size; ++i){
        op = strips->op.op[i];
        BOR_ISET_FOR_EACH(&op->add_eff, fact_id)
            is_not_static[fact_id] = 1;
        BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
            is_not_static[fact_id] = 1;
        // TODO: Cond-eff
    }

    // Move them to a set
    borISetInit(&static_facts);
    for (int i = 0; i < strips->fact.fact_size; ++i){
        if (!is_not_static[i])
            borISetAdd(&static_facts, i);
    }

    // Find static but unreachable facts
    borISetInit(&static_unreachable_facts);
    borISetUnion(&static_unreachable_facts, &static_facts);
    borISetMinus(&static_unreachable_facts, &strips->init);

    removeOpsWithUnreachablePre(strips, &static_unreachable_facts);
    removeFacts(strips, &static_facts);
    BOR_ISET_FOR_EACH(&static_unreachable_facts, fact_id)
        printf("S: %s\n", pddlFactToStr(strips->pddl,
                    strips->fact.fact[fact_id]));

    BOR_FREE(is_not_static);
    borISetFree(&static_facts);
    borISetFree(&static_unreachable_facts);
}

static void removeFacts(pddl_strips_t *strips, const bor_iset_t *rm)
{
    // TODO: Cond-eff
}

static void removeOpsWithUnreachablePre(pddl_strips_t *strips,
                                        const bor_iset_t *unreachable_facts)
{
}
