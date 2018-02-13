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

#include <boruvka/timer.h>
#include "pddl/strips.h"
#include "pddl/mgroup.h"
#include "pddl/mutex.h"
#include "err.h"
#include "assert.h"
#include "profile.h"

/** Prunes irrelevant facts and operators.
 *  Implemented in strips_irrelevance.c */
int _pddlStripsPruneIrrelevant(pddl_strips_t *strips,
                               const pddl_strips_prune_config_t *cfg);


struct prune;

/** Should return:
 *      1  - for successful pruning
 *      0  - for successful run without pruning
 *      -1 - error
 *      -2 - abort due to time limit
 */
typedef int (*prune_alg_fn)(struct prune *prune);

struct alg {
    prune_alg_fn fn;
    int run; /*!< True if this algorithm should be run */
};
typedef struct alg alg_t;

struct prune {
    const pddl_strips_prune_config_t *cfg;
    pddl_strips_t *strips;

    alg_t alg[5];
    int alg_size;

    int *op; /*!< Pre-allocated array for operators */
    bor_timer_t timer;
    float time_elapsed;
    float time_remain;
};
typedef struct prune prune_t;

static int pruneOpArr(prune_t *prune)
{
    int change = 0;

    for (int i = 0; i < prune->strips->op.op_size; ++i){
        if (prune->op[i]){
            change = 1;
            break;
        }
    }

    if (change)
        pddlStripsOpsDelOps(&prune->strips->op, prune->op);
    return change;
}

static int pruneIrrelevance(prune_t *prune)
{
    int ret;

    ret = _pddlStripsPruneIrrelevant(prune->strips, prune->cfg);
    if (ret > 0)
        ret = 1;
    return ret;
}

static int pruneHMutex(prune_t *prune)
{
    pddl_strips_t *strips = prune->strips;
    pddl_mutexes_t *mutex = &strips->mutex;
    int ret;

    // Reuse array for operators
    bzero(prune->op, sizeof(int) * strips->op.op_size);

    pddlMutexesFree(mutex);
    pddlMutexesInit(mutex);
    ret = pddlMutexesHm(mutex, prune->cfg->h_mutex, strips, prune->op,
                        prune->cfg->max_mem, prune->time_remain);

    if (ret == 0){
        if (pruneOpArr(prune))
            ret = 1;
        INFO("O: %d, F: %d :: h^%d mutexes (mutexes: %d, change: %d).",
             strips->op.op_size, strips->fact.fact_size, prune->cfg->h_mutex,
             strips->mutex.mutex_size, ret);
    }

    return ret;
}

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
    const pddl_mgroup_t *mg;
    bor_iset_t mpre, mdel, mpredel, madd;

    // Reuse array for operators
    bzero(prune_op, sizeof(int) * strips->op.op_size);

    borISetInit(&mpre);
    borISetInit(&mdel);
    borISetInit(&mpredel);
    borISetInit(&madd);

    PDDL_MGROUPS_FOR_EACH(mgs, mg){
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

static int pruneFAMGroup(prune_t *prune)
{
    pddl_strips_t *strips = prune->strips;
    pddl_mgroups_t *mgroup = &strips->mgroup;
    int ret;

    pddlMGroupsFree(mgroup);
    pddlMGroupsInit(mgroup);
    ret = pddlMGroupsFA(strips, mgroup);

    if (ret == 0){
        if (pruneWithMGroups(strips, mgroup, prune->cfg, prune->op))
            ret = 1;

        // TODO: Count dead-end operators
        INFO("O: %d, F: %d :: fam-groups (mgroups: %d, change: %d).",
             strips->op.op_size, strips->fact.fact_size,
             strips->mgroup.mgroup_size, ret);
        pddlMGroupsFinalize(&strips->mgroup, strips);
    }

    return ret;
}

static bor_iset_t *mutexTableNew(const pddl_strips_t *strips)
{
    bor_iset_t *table;
    const pddl_mutex_t *m;
    //const pddl_mgroup_t *mg;

    table = BOR_CALLOC_ARR(bor_iset_t, strips->fact.fact_size);
    PDDL_MUTEXES_FOR_EACH(&strips->mutex, m){
        const bor_iset_t *f = &m->fact;
        if (borISetSize(f) == 2){
            borISetAdd(&table[borISetGet(f, 0)], borISetGet(f, 1));
            borISetAdd(&table[borISetGet(f, 1)], borISetGet(f, 0));
        }
    }

    /* TODO: This is useless (but time demanding) if h^2 mutexes are
     *       inferred and strips->mgroup contains only fa-mgroups.
     *       Parametrize this somehow to recongnize when this situation
     *       happens. For now disabled.
    PDDL_MGROUPS_FOR_EACH(&strips->mgroup, mg){
        const bor_iset_t *f = &mg->fact;
        for (int i = 0; i < f->size; ++i){
            for (int j = i + 1; j < f->size; ++j){
                borISetAdd(&table[borISetGet(f, i)], borISetGet(f, j));
                borISetAdd(&table[borISetGet(f, j)], borISetGet(f, i));
            }
        }
    }
    */

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
            if (borISetSize(&remain) == 0){
                borISetFree(&remain);
                borISetFree(&mutex_facts);
                return -1;
            }
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

static int pruneDisambiguation(prune_t *prune)
{
    pddl_strips_t *strips = prune->strips;
    bor_iset_t *mutex;
    int ret;
    int change = 0;

    if (strips->mgroup.mgroup_size == 0)
        return 0;

    mutex = mutexTableNew(strips);
    for (int oi = 0; oi < strips->op.op_size; ++oi){
        pddl_strips_op_t *op = strips->op.op[oi];
        change |= disambiguatePre(op, mutex, &strips->mgroup);
    }

    ret = disambiguateSet(&strips->goal, mutex, &strips->mgroup);
    if (ret < 0){
        strips->goal_is_unreachable = 1;
        ret = 0;
        INFO("O: %d, F: %d :: Disambiguation done -- goal is unreachable.",
             strips->op.op_size, strips->fact.fact_size);
    }else{
        ret |= change;
        INFO("O: %d, F: %d :: Disambiguation done (change: %d).",
             strips->op.op_size, strips->fact.fact_size, ret);
    }

    mutexTableDel(mutex, strips);
    return ret;
}

static void updateTimer(prune_t *prune)
{
    borTimerStop(&prune->timer);
    prune->time_elapsed = borTimerElapsedInSF(&prune->timer);

    if (prune->cfg->max_time <= 0.f){
        prune->time_remain = 1E10;
    }else{
        prune->time_remain = prune->cfg->max_time - prune->time_elapsed;
    }
}

static void enableAlgs(prune_t *prune, int except)
{
    for (int i = 0; i < prune->alg_size; ++i){
        if (i != except)
            prune->alg[i].run = 1;
    }
}

static void disableAllAlgs(prune_t *prune)
{
    for (int i = 0; i < prune->alg_size; ++i)
        prune->alg[i].run = 0;
}

static void checkTimer(prune_t *prune)
{
    updateTimer(prune);

    if (prune->time_remain < 0.f){
        disableAllAlgs(prune);
        INFO("  == Time limit for pruning was exceeded (%f/%f)",
             prune->time_elapsed, prune->cfg->max_time);
    }
}

static int pruneAlg(prune_t *prune, int alg_id)
{
    alg_t *alg = prune->alg + alg_id;
    int ret;

    updateTimer(prune);

    alg->run = 0;
    ret = alg->fn(prune);
    if (ret == 1){
        enableAlgs(prune, alg_id);

    }else if (ret == -1){
        TRACE_RET(-1);

    }else if (ret == -2){
        disableAllAlgs(prune);
    }

    checkTimer(prune);

    return 0;
}

int pddlStripsPrune(pddl_strips_t *strips,
                    const pddl_strips_prune_config_t *cfg)
{
    prune_t prune;
    int cont, ret = 0;

    if (strips->goal_is_unreachable)
        return 0;

    prune.cfg = cfg;
    prune.strips = strips;

    prune.alg_size = 0;
    if (cfg->irrelevance || cfg->static_facts)
        prune.alg[prune.alg_size++].fn = pruneIrrelevance;
    if (cfg->h_mutex > 0)
        prune.alg[prune.alg_size++].fn = pruneHMutex;
    if (cfg->fa_mgroup)
        prune.alg[prune.alg_size++].fn = pruneFAMGroup;
    if (cfg->disambiguation && cfg->fa_mgroup)
        prune.alg[prune.alg_size++].fn = pruneDisambiguation;

    for (int i = 0; i < prune.alg_size; ++i)
        prune.alg[i].run = 1;

    prune.op = BOR_ALLOC_ARR(int, strips->op.op_size);
    borTimerStart(&prune.timer);
    updateTimer(&prune);

    do {
        cont = 0;
        for (int i = 0; i < prune.alg_size; ++i){
            if (prune.alg[i].run){
                if ((ret = pruneAlg(&prune, i)) != 0){
                    cont = 0;
                    break;
                }
                cont = 1;
            }
        }
    } while (cfg->fixpoint && cont && !strips->goal_is_unreachable);

    if (ret == 0)
        INFO2("The STRIPS problem is pruned.");

    BOR_FREE(prune.op);
    return ret;
}

