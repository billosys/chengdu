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


static int pruneOpWithMGroup(const pddl_mgroup_t *mg,
                             const bor_iset_t *mpre,
                             const bor_iset_t *mdel,
                             const bor_iset_t *mpredel,
                             const bor_iset_t *madd,
                             const pddl_strips_prune_config_t *cfg)
{
    // If the precondition or add effect contains more one fact
    // from the mutex group, it is clearly an unreachable operator
    if (borISetSize(mpre) > 1 || borISetSize(madd) > 1)
        return 1;

    // The dead-end operators are those that delete all fact from
    // the fact-alternating mutex group even though it is required
    // that one of the facts is part of the goal.
    if (cfg->fa_mgroup_dead_end
            && mg->is_fa
            && mg->is_goal
            && borISetSize(madd) == 0
            && borISetSize(mpredel) > 0){
        return 1;
    }

    return 0;
}

static int pruneWithMGroups(pddl_strips_t *strips,
                            const pddl_mgroups_t *mgs,
                            const pddl_strips_prune_config_t *cfg)
{
    int *prune_op = BOR_CALLOC_ARR(int, strips->op.op_size);
    int change = 0;
    bor_iset_t mpre, mdel, mpredel, madd;

    borISetInit(&mpre);
    borISetInit(&mdel);
    borISetInit(&mpredel);
    borISetInit(&madd);

    for (int mgi = 0; mgi < mgs->size; ++mgi){
        const pddl_mgroup_t *mg = mgs->g + mgi;
        for (int opi = 0; opi < strips->op.op_size; ++opi){
            pddl_strips_op_t *op = strips->op.op[opi];
            if (op == NULL || prune_op[opi])
                continue;

            borISetIntersect2(&mpre, &mg->fact, &op->pre);
            borISetIntersect2(&mdel, &mg->fact, &op->del_eff);
            borISetIntersect2(&mpredel, &mpre, &mdel);
            borISetIntersect2(&madd, &mg->fact, &op->add_eff);

            prune_op[opi] = pruneOpWithMGroup(mg, &mpre, &mdel, &mpredel,
                                              &madd, cfg);
            change |= prune_op[opi];

            // Disambiguate delete effect
            if (cfg->disambiguation
                    && borISetSize(&mpre) > 0
                    && borISetSize(&mdel) > 0
                    && borISetSize(&mpredel) != borISetSize(&mdel)){
                WARN("Disambiguation of delete effect of %s", op->name);
                borISetMinus(&mdel, &mpredel);
                borISetMinus(&op->del_eff, &mdel);
            }
        }
    }

    pddlStripsOpsDel(&strips->op, prune_op);

    BOR_FREE(prune_op);

    borISetFree(&mpre);
    borISetFree(&mdel);
    borISetFree(&mpredel);
    borISetFree(&madd);

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

        // TODO: Disable this for now
#if 0
        if (cfg->h_mutex_bw > 0){
            pddl_strips_t *dual = pddlStripsDual(strips);
            bzero(prune_op, sizeof(int) * strips->op.op_size);
            if (pddlMutexesHm(cfg->h_mutex_bw, dual, NULL, prune_op) != 0){
                pddlMutexesFree(&mutex);
                pddlMGroupsFree(&mgroup);
                BOR_FREE(prune_op);
                TRACE_RET(-1);
            }
            pddlStripsDel(dual);

            pddlStripsOpsDel(&strips->op, prune_op);
        }
#endif

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
