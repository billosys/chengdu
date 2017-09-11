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
#include "assert.h"

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
                            const pddl_strips_prune_config_t *cfg,
                            int *prune_op)
{
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

    pddlStripsOpsDelOps(&strips->op, prune_op);

    borISetFree(&mpre);
    borISetFree(&mdel);
    borISetFree(&mpredel);
    borISetFree(&madd);

    return change;
}

static int pruneFAMGroup(pddl_strips_t *strips,
                         const pddl_strips_prune_config_t *cfg,
                         int *prune_op,
                         int *change)
{
    pddl_mgroups_t *mgroup = &strips->mgroup;
    int ret;

    // Reuse array for operators
    bzero(prune_op, sizeof(int) * strips->op.op_size);

    pddlMGroupsFree(mgroup);
    pddlMGroupsInit(mgroup);
    if ((ret = pddlMGroupsFA(strips, mgroup)) == 0)
        *change |= pruneWithMGroups(strips, mgroup, cfg, prune_op);

    return ret;
}

static int pruneOpArr(pddl_strips_t *strips,
                      const int *prune_op)
{
    int change = 0;

    for (int i = 0; i < strips->op.op_size; ++i){
        if (prune_op[i]){
            change = 1;
            break;
        }
    }

    if (change)
        pddlStripsOpsDelOps(&strips->op, prune_op);
    return change;
}

static int pruneHMutex(pddl_strips_t *strips,
                       const pddl_strips_prune_config_t *cfg,
                       int *prune_op,
                       int *change)
{
    pddl_mutexes_t *mutex = &strips->mutex;
    int ret;

    // Reuse array for operators
    bzero(prune_op, sizeof(int) * strips->op.op_size);

    pddlMutexesFree(mutex);
    pddlMutexesInit(mutex);
    if ((ret = pddlMutexesHm(cfg->h_mutex, strips, mutex, prune_op)) == 0)
        *change |= pruneOpArr(strips, prune_op);

    return ret;
}


static bor_iset_t *mutexTableNew(const pddl_strips_t *strips)
{
    bor_iset_t *table;
    const pddl_mutex_t *m;
    const pddl_mgroup_t *mg;

    table = BOR_CALLOC_ARR(bor_iset_t, strips->fact.fact_size);
    PDDL_MUTEXES_FOR_EACH(&strips->mutex, m){
        const bor_iset_t *f = &m->fact;
        if (borISetSize(f) == 2){
            borISetAdd(&table[borISetGet(f, 0)], borISetGet(f, 1));
            borISetAdd(&table[borISetGet(f, 1)], borISetGet(f, 0));
        }
    }

    PDDL_MGROUPS_FOR_EACH(&strips->mgroup, mg){
        const bor_iset_t *f = &mg->fact;
        for (int i = 0; i < f->size; ++i){
            for (int j = i + 1; j < f->size; ++j){
                borISetAdd(&table[borISetGet(f, i)], borISetGet(f, j));
                borISetAdd(&table[borISetGet(f, j)], borISetGet(f, i));
            }
        }
    }

    return table;
}

static void mutexTableDel(bor_iset_t *t, const pddl_strips_t *strips)
{
    for (int i = 0; i < strips->fact.fact_size; ++i)
        borISetFree(t + i);
    if (t != NULL)
        BOR_FREE(t);
}

static int disambiguateSet(bor_iset_t *set,
                           const bor_iset_t *mutex,
                           const pddl_mgroups_t *mgroups)
{
    bor_iset_t mutex_facts;
    bor_iset_t remain;
    int fact_id;
    int change = 0, local_change;
    const pddl_mgroup_t *mg;

    borISetInit(&mutex_facts);
    BOR_ISET_FOR_EACH(set, fact_id)
        borISetUnion(&mutex_facts, &mutex[fact_id]);

    borISetInit(&remain);
    do {
        local_change = 0;
        PDDL_MGROUPS_FOR_EACH(mgroups, mg){
            if (!mg->is_exactly_1)
                continue;
            borISetMinus2(&remain, &mg->fact, &mutex_facts);
            if (borISetSize(&remain) == 0)
                return -1;
            if (borISetSize(&remain) == 1
                    && !borISetIn(borISetGet(&remain, 0), set)){
                borISetAdd(set, borISetGet(&remain, 0));
                borISetUnion(&mutex_facts, &mutex[borISetGet(&remain, 0)]);
                change = local_change = 1;
            }
        }
    } while (local_change);


    borISetFree(&remain);
    borISetFree(&mutex_facts);

    return change;
}

static int disambiguatePre(pddl_strips_op_t *op,
                           const bor_iset_t *mutex,
                           const pddl_mgroups_t *mgroups)
{
    int change;

    change = disambiguateSet(&op->pre, mutex, mgroups);
    ASSERT(change >= 0);
    if (change)
        pddlStripsOpNormalize(op);

    return change;
}

static int disambiguate(pddl_strips_t *strips,
                        const pddl_strips_prune_config_t *cfg,
                        int *change)
{
    bor_iset_t *mutex;
    int ret;

    if (strips->mgroup.size == 0)
        return 0;

    mutex = mutexTableNew(strips);
    for (int oi = 0; oi < strips->op.op_size; ++oi){
        pddl_strips_op_t *op = strips->op.op[oi];
        *change |= disambiguatePre(op, mutex, &strips->mgroup);
    }

    ret = disambiguateSet(&strips->goal, mutex, &strips->mgroup);
    if (ret < 0){
        strips->goal_is_unreachable = 1;
        *change = 1;
    }else{
        *change |= ret;
    }

    mutexTableDel(mutex, strips);
    return 0;
}

int pddlStripsPrune(pddl_strips_t *strips,
                    const pddl_strips_prune_config_t *cfg)
{
    int change;
    int *prune_op;

    if (strips->goal_is_unreachable)
        return 0;

    INFO("Start pruning of the STRIPS problem (num-ops: %d).",
         strips->op.op_size);
    prune_op = BOR_ALLOC_ARR(int, strips->op.op_size);

    do {
        change = 0;

        if (cfg->irrelevance || cfg->static_facts)
            change |= _pddlStripsPruneIrrelevant(strips, cfg);

        if (cfg->h_mutex > 0){
            if (pruneHMutex(strips, cfg, prune_op, &change) != 0){
                BOR_FREE(prune_op);
                TRACE_RET(-1);
            }
            INFO("Pruning using h^%d mutexes done"
                 " (num-ops: %d, num-mutexes: %d).",
                 cfg->h_mutex, strips->op.op_size, strips->mutex.size);
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

            pddlStripsOpsDelOps(&strips->op, prune_op);
        }
#endif

        if (cfg->fa_mgroup){
            if (pruneFAMGroup(strips, cfg, prune_op, &change) != 0){
                BOR_FREE(prune_op);
                TRACE_RET(-1);
            }
            INFO("Pruning using fam-groups done"
                 " (num-ops: %d, num-mgroups: %d).",
                 strips->op.op_size, strips->mgroup.size);
        }

        if (cfg->disambiguation && cfg->fa_mgroup){
            if (disambiguate(strips, cfg, &change) != 0){
                BOR_FREE(prune_op);
                TRACE_RET(-1);
            }
            INFO("Disambiguation of operators done (num-ops: %d).",
                 strips->op.op_size);
        }
        if (cfg->fixpoint && change)
            INFO2("  == Fixpoint not reached, continuing to prune... ==");
    } while (cfg->fixpoint && change && !strips->goal_is_unreachable);

    BOR_FREE(prune_op);

    // TODO: remove identical/dominated operators
    //       (don't forget to keep the one with the minimal cost)

    INFO2("The STRIPS problem is pruned.");
    return 0;
}
