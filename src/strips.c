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

int pddlStripsFromPDDLFull(pddl_strips_t *strips,
                           const pddl_t *pddl,
                           unsigned flags);

int pddlStripsFromPDDL(pddl_strips_t *strips, const pddl_t *pddl,
                       unsigned flags)
{
    if (flags & PDDL_STRIPS_GROUND_FULL)
        return pddlStripsFromPDDLFull(strips, pddl, flags);
    return -1;
}

void pddlStripsFree(pddl_strips_t *strips)
{
}

void pddlStripsDump(const pddl_strips_t *strips, FILE *fout)
{
    int i;

    fprintf(fout, "Op[%d]:\n", strips->op.size);
    for (i = 0; i < strips->op.size; ++i){
        fprintf(fout, "  %s, cost: %d\n",
                strips->op.op[i].name,
                strips->op.op[i].cost);
        fprintf(fout, "    pre:");
        // TODO: pre, eff, cond_eff
    }
    // TODO: facts, init, goal
}
