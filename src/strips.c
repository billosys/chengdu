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

/** Implemented in strips_ground.c */
void _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl);

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

    // TODO: remove static facts
    // TODO: remove identical operators (don't forget to keep the one with
    // the minimal cost)
    // TODO: causal graph
    // TODO: pruning
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
