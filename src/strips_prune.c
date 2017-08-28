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
#include "err.h"

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
    if (!cfg->fa_mgroup_dead_end || !mg->is_fa || !mg->is_goal)
        return 0;
    return !borISetIntersectionSizeAtLeast(&op->add_eff, &mg->fact, 1)
            && borISetIntersectionSizeAtLeast3(&op->pre, &op->del_eff,
                                               &mg->fact, 1);
}


static int pruneWithMGroups(pddl_strips_t *strips,
                            const pddl_mgroups_t *mgs,
                            const pddl_strips_prune_config_t *cfg)
{
    int *prune_op = BOR_CALLOC_ARR(int, strips->op.op_size);
    int change = 0;

    for (int mgi = 0; mgi < mgs->size; ++mgi){
        const pddl_mgroup_t *mg = mgs->g + mgi;
        for (int opi = 0; opi < strips->op.op_size; ++opi){
            const pddl_strips_op_t *op = strips->op.op[opi];
            if (op == NULL || prune_op[opi])
                continue;
            if (mgroupOpIsUnreachable(op, &mg->fact)
                    || mgroupOpIsDeadEnd(op, mg, cfg)){
                prune_op[opi] = 1;
                change = 1;
            }
        }
    }

    pddlStripsOpsDel(&strips->op, prune_op);

    BOR_FREE(prune_op);

    return change;
}

static int pruneWithMutexes(pddl_strips_t *strips,
                            const pddl_mutexes_t *ms,
                            const pddl_strips_prune_config_t *cfg,
                            int *prune_op)
{
    int change = 0;

    for (int i = 0; i < strips->op.op_size; ++i){
        if (prune_op[i]){
            change = 1;
            break;
        }
    }

    if (change)
        pddlStripsOpsDel(&strips->op, prune_op);
    return change;
}

int pddlStripsPrune(pddl_strips_t *strips,
                    const pddl_strips_prune_config_t *cfg)
{
    pddl_mgroups_t mgroup;
    pddl_mutexes_t mutex;
    int change;
    int *prune_op;

    prune_op = BOR_ALLOC_ARR(int, strips->op.op_size);

    do {
        pddlMGroupsInit(&mgroup);
        pddlMutexesInit(&mutex);
        change = 0;

        if (cfg->irrelevance || cfg->static_facts)
            change |= _pddlStripsPruneIrrelevant(strips, cfg);

        if (cfg->h_mutex > 0){
            bzero(prune_op, sizeof(int) * strips->op.op_size);
            if (pddlMutexesHm(cfg->h_mutex, strips, &mutex, prune_op) != 0){
                pddlMutexesFree(&mutex);
                pddlMGroupsFree(&mgroup);
                BOR_FREE(prune_op);
                TRACE_RET(-1);
            }

            change |= pruneWithMutexes(strips, &mutex, cfg, prune_op);
        }

        // TODO
        if (cfg->h_mutex_bw > 0){
        }

        if (cfg->fa_mgroup){
            if (pddlMGroupsFA(strips, &mgroup) != 0){
                pddlMutexesFree(&mutex);
                pddlMGroupsFree(&mgroup);
                BOR_FREE(prune_op);
                TRACE_RET(-1);
            }

            change |= pruneWithMGroups(strips, &mgroup, cfg);
        }

        // TODO
        if (cfg->disambiguation){
        }

        pddlMutexesFree(&mutex);
        pddlMGroupsFree(&mgroup);
    } while (cfg->fixpoint && change);

    BOR_FREE(prune_op);

    if (strips->goal_is_unreachable)
        pddlStripsMakeUnsolvable(strips);
    // TODO: remove identical/dominated operators
    //       (don't forget to keep the one with the minimal cost)

    return 0;
}
