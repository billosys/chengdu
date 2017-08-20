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

#include "pddl/strips.h"
#include "pddl/mgroup.h"
#include "pddl/mutex.h"

/** Prunes irrelevant facts and operators.
 *  Implemented in strips_irrelevance.c */
int _pddlStripsPruneIrrelevant(pddl_strips_t *strips,
                               const pddl_strips_prune_config_t *cfg);

_bor_inline int mgroupOpIsUnreachable(const pddl_strips_op_t *op,
                                      const bor_iset_t *mg)
{
    return borISetIntersectionSizeAtLeast(mg, &op->pre, 2)
            || borISetIntersectionSizeAtLeast(mg, &op->add_eff, 2);
}

_bor_inline int mgroupOpIsDeadEnd(const pddl_strips_op_t *op,
                                  const pddl_mgroup_t *mg,
                                  const pddl_strips_prune_config_t *cfg)
{
    if (!cfg->fa_mgroup_dead_end || !mg->is_fa)
        return 0;
    return !borISetIntersectionSizeAtLeast(&op->add_eff, &mg->fact, 1)
            && borISetIntersectionSizeAtLeast3(&op->pre, &op->del_eff,
                                               &mg->fact, 1);
}


static void pruneWithMGroups(const pddl_strips_t *strips,
                             const pddl_mgroups_t *mgs,
                             const pddl_strips_prune_config_t *cfg,
                             int *prune_op)
{
    for (int opi = 0; opi < strips->op.op_size; ++opi){
        const pddl_strips_op_t *op = strips->op.op[opi];
        if (op == NULL || prune_op[opi])
            continue;
        for (int mgi = 0; mgi < mgs->size; ++mgi){
            const pddl_mgroup_t *mg = mgs->g + mgi;
            if (mgroupOpIsUnreachable(op, &mg->fact)
                    || mgroupOpIsDeadEnd(op, mg, cfg)){
                prune_op[opi] = 1;
            }
        }
    }
}

void pddlStripsPrune(pddl_strips_t *strips,
                     const pddl_strips_prune_config_t *cfg)
{
    if (cfg->irrelevance)
        _pddlStripsPruneIrrelevant(strips, cfg);
    if (strips->goal_is_unreachable)
        pddlStripsMakeUnsolvable(strips);
    // TODO: remove identical/dominated operators
    //       (don't forget to keep the one with the minimal cost)
}
