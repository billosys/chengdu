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

pddl_strips_t *pddlStripsGround(const pddl_t *pddl, unsigned flags)
{
    pddl_strips_t *strips;

    strips = BOR_ALLOC(pddl_strips_t);
    bzero(strips, sizeof(*strips));
    strips->pddl = pddl;
    pddlFactsInit(&strips->fact);
    pddlStripsOpsInit(&strips->op);
    pddlFactIdArrInit(&strips->init);
    pddlFactIdArrInit(&strips->goal);

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
    pddlFactIdArrFree(&strips->init);
    pddlFactIdArrFree(&strips->goal);
    BOR_FREE(strips);
}

void pddlStripsDump(const pddl_strips_t *strips, FILE *fout)
{
    fprintf(fout, "Fact[%d]:\n", strips->fact.fact_size);
    pddlFactsPrintSorted(strips->pddl, &strips->fact, fout);

    fprintf(fout, "Op[%d]:\n", strips->op.op_size);
    pddlStripsOpsPrint(strips->pddl, &strips->fact, &strips->op, fout);
}
