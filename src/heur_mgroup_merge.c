/***
 * cpddl
 * -------
 * Copyright (c)2018 Daniel Fiser <danfis@danfis.cz>,
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
#include <boruvka/sort.h>
#include "pddl/sync_product_prep.h"
#include "pddl/heur_mgroup_merge.h"
#include "pddl/mgroup_op_bipartite_graph.h"
#include "pddl/sync_product.h"
#include "pddl/util.h"
#include "err.h"

#define SCALE_LIMIT 10000
#define SCALE_HARD_LIMIT 100000

struct mgroup_sort_item {
    int mgroup_id;
    int eff;
    int pre;
};
typedef struct mgroup_sort_item mgroup_sort_item_t;

static int mgroupSortItemCmp(const void *a, const void *b, void *_)
{
    const mgroup_sort_item_t *i1 = a;
    const mgroup_sort_item_t *i2 = b;
    int cmp = i2->eff - i1->eff;
    if (cmp == 0)
        cmp = i2->pre - i1->pre;
    return cmp;
}

static void sortMGroupsForMerge(const bor_iset_t *mgroup,
                                const bor_iset_t *mgroup_facts,
                                const bor_iset_t *mgroup_ops_pre,
                                const pddl_strips_t *strips,
                                const pddl_strips_cross_ref_t *cref,
                                mgroup_sort_item_t *list)
{
    for (int i = 0; i < strips->mgroup.mgroup_size; ++i){
        const pddl_mgroup_t *m = strips->mgroup.mgroup + i;
        list[i].mgroup_id = i;
        if (borISetIn(i, mgroup)){
            list[i].eff = -1;
            list[i].pre = -1;
        }else{
            list[i].eff = borISetIntersectionSize(mgroup_facts, &m->fact);
            list[i].pre = borISetIntersectionSize(mgroup_ops_pre,
                                                  &cref->mgroup[i].op_add);
        }
    }

    borSort(list, strips->mgroup.mgroup_size, sizeof(*list),
            mgroupSortItemCmp, NULL);
}

static void addMerge(pddl_heur_mgroup_merge_t *h,
                     const bor_iset_t *merge,
                     int max_nodes)
{
    for (int i = 0; i < h->merge_size; ++i){
        if (borISetEq(merge, &h->merge[i].mgroup))
            return;
    }

    pddl_heur_mgroup_merge_merge_t *m = h->merge + h->merge_size++;
    borISetUnion(&m->mgroup, merge);
    m->max_nodes = max_nodes;
    pddlLandmarkSeqInit(&m->ldm_seq);
}

static void findMergesMGroup(pddl_heur_mgroup_merge_t *h,
                             int mgroup_id,
                             const pddl_strips_t *strips,
                             const pddl_strips_cross_ref_t *cref)
{
    BOR_ISET(facts);
    BOR_ISET(pre_ops);
    mgroup_sort_item_t *mgroup_merge;
    pddl_sync_product_prep_t sp;
    int change = 1;

    borISetUnion(&facts, &strips->mgroup.mgroup[mgroup_id].fact);
    borISetUnion(&pre_ops, &cref->mgroup[mgroup_id].op_pre);
    pddlSyncProductPrepInit(&sp, mgroup_id, strips);

    mgroup_merge = BOR_ALLOC_ARR(mgroup_sort_item_t,
                                 strips->mgroup.mgroup_size);

    while (change){
        change = 0;
        sortMGroupsForMerge(&sp.mgroup, &facts, &pre_ops,
                            strips, cref, mgroup_merge);

        for (int i = 0; i < strips->mgroup.mgroup_size; ++i){
            mgroup_sort_item_t *m = mgroup_merge + i;
            if (m->eff < 0 || m->pre < 0)
                break;
            if (pddlSyncProductPrepExtend(&sp, m->mgroup_id,
                                          strips, h->max_mem) == 0){
                borISetUnion(&facts, &strips->mgroup.mgroup[m->mgroup_id].fact);
                borISetUnion(&pre_ops, &cref->mgroup[m->mgroup_id].op_pre);
                INFO("h-mgroup-merge:   Adding mgroup %d, num-nodes: %d",
                     m->mgroup_id, sp.node_size);
                change = 1;
                break;
            }
        }
    }

    if (borISetSize(&sp.mgroup) > 1)
        addMerge(h, &sp.mgroup, sp.node_size);

    BOR_FREE(mgroup_merge);
    pddlSyncProductPrepFree(&sp);
    borISetFree(&facts);
    borISetFree(&pre_ops);
}

static void findMerges(pddl_heur_mgroup_merge_t *h,
                       const pddl_strips_t *strips,
                       const pddl_strips_cross_ref_t *cref)
{
    int num_goal_mgroups = 0;

    for (int mi = 0; mi < strips->mgroup.mgroup_size; ++mi){
        if (strips->mgroup.mgroup[mi].is_goal)
            ++num_goal_mgroups;
    }
    h->merge = BOR_CALLOC_ARR(pddl_heur_mgroup_merge_merge_t,
                              num_goal_mgroups);

    for (int mi = 0; mi < strips->mgroup.mgroup_size; ++mi){
        const pddl_mgroup_t *m = strips->mgroup.mgroup + mi;
        if (m->is_goal){
            INFO("h-mgroup-merge: Finding merge for mgroup %d (%d facts)",
                 mi, borISetSize(&m->fact));
            findMergesMGroup(h, mi, strips, cref);
        }
    }
}

static void costPart(pddl_heur_mgroup_merge_t *h,
                     const pddl_strips_cross_ref_t *cref)
{
    BOR_ISET(mg_ops);
    int *ops, mgi, opi, scale;

    // For each operator, count number of merges it appears in.
    ops = BOR_CALLOC_ARR(int, h->strips->op.op_size);
    for (int i = 0; i < h->merge_size; ++i){
        borISetEmpty(&mg_ops);
        BOR_ISET_FOR_EACH(&h->merge[i].mgroup, mgi)
            borISetUnion(&mg_ops, &cref->mgroup[mgi].op_del_add);

        BOR_ISET_FOR_EACH(&mg_ops, opi)
            ops[opi] += 1;
    }

    // Compute scaling factor
    scale = 1;
    for (int i = 0; scale < SCALE_LIMIT && i < h->strips->op.op_size; ++i){
        if (ops[i] > 1)
            scale = pddlLCM(scale, ops[i]);
    }
    if (scale < 0 || scale > SCALE_HARD_LIMIT)
        scale = SCALE_LIMIT;
    INFO("h-mgroup-merge: Operator cost scaling factor: %d", scale);

    // Apply scaling factor and apply uniform cost partitioning
    h->op_cost_scale = scale;
    for (int i = 0; i < h->strips->op.op_size; ++i){
        h->strips->op.op[i]->cost *= scale;
        if (ops[i] > 1){
            h->strips->op.op[i]->cost /= ops[i];
        }
    }

    BOR_FREE(ops);
    borISetFree(&mg_ops);
}

static void addValue(pddl_heur_mgroup_merge_merge_t *m,
                     const pddl_sync_product_t *sprod,
                     const pddl_sync_product_node_t *snode,
                     int value)
{
    if (m->value_size == m->value_alloc){
        if (m->value_alloc == 0)
            m->value_alloc = 128;
        m->value_alloc *= 2;
        m->value = BOR_REALLOC_ARR(m->value, pddl_heur_mgroup_merge_value_t,
                                   m->value_alloc);
    }

    pddl_heur_mgroup_merge_value_t *v = m->value + m->value_size++;

    borISetInit(&v->fact);
    for (int i = 0; i < sprod->fact_size; ++i)
        borISetAdd(&v->fact, snode->fact[i]);

    v->value = value;
}

static void computeMerge(pddl_heur_mgroup_merge_t *h,
                         const pddl_strips_cross_ref_t *cref,
                         pddl_heur_mgroup_merge_merge_t *merge)
{
    pddl_sync_product_t sp;
    BOR_IARR(dist);
    int init_node;

    int mi;
    BOR_ISET_FOR_EACH(&merge->mgroup, mi){
        fprintf(stderr, " %d(%d)", mi,
                borISetSize(&h->strips->mgroup.mgroup[mi].fact));
    }
    fprintf(stderr, "\n");
    pddlSyncProductInitMaxNodes(&sp, &merge->mgroup, h->strips, cref,
                                merge->max_nodes);
    INFO("h-mgroup-merge: Computed sync product of %d mgroups, nodes: %d",
         borISetSize(&merge->mgroup), sp.node_size);

    pddlSyncProductGoalDistance(&sp, &dist);
    for (int i = 0; i < sp.node_size; ++i){
        const pddl_sync_product_node_t *n = sp.node + i;
        addValue(merge, &sp, n, borIArrGet(&dist, i));
    }
    INFO2("h-mgroup-merge: Goal Distances computed");

    for (init_node = 0; !sp.node[init_node].is_init
                && init_node < sp.node_size; ++init_node);
    pddlSyncProductFindLandmarks(&sp, cref, init_node,
                                 NULL, &merge->ldm_seq, NULL, NULL);
    INFO2("h-mgroup-merge: Landmarks computed");

    pddlSyncProductFree(&sp);
    borIArrFree(&dist);
}

void pddlHeurMGroupMergeInit(pddl_heur_mgroup_merge_t *h,
                             const pddl_strips_t *_strips,
                             int use_cost_part,
                             size_t max_mem)
{
    pddl_strips_t *strips = pddlStripsClone(_strips);
    pddl_strips_cross_ref_t cref;

    INFO("h-mgroup-merge: cost-part: %d, max_mem: %lu",
         use_cost_part, (unsigned long)max_mem);
    pddlStripsCompleteMGroups(strips);
    pddlStripsMakeExactlyOneMGroups(strips);
    pddlStripsCrossRefInit(&cref, strips);
    INFO2("h-mgroup-merge: Initialized.");

    bzero(h, sizeof(*h));
    h->strips = strips;
    h->op_cost_scale = 1;
    h->max_mem = max_mem;

    findMerges(h, strips, &cref);
    INFO("h-mgroup-merge: %d merges planned.", h->merge_size);

    if (use_cost_part){
        costPart(h, &cref);
        INFO2("h-mgroup-merge: Cost partitioning done.");
    }

    for (int i = 0; i < h->merge_size; ++i)
        computeMerge(h, &cref, h->merge + i);

    pddlStripsCrossRefFree(&cref);
}

void pddlHeurMGroupMergeFree(pddl_heur_mgroup_merge_t *h)
{
    for (int i = 0; i < h->merge_size; ++i){
        borISetFree(&h->merge[i].mgroup);
        for (int j = 0; j < h->merge[i].value_size; ++j)
            borISetFree(&h->merge[i].value[j].fact);
        if (h->merge[i].value != NULL)
            BOR_FREE(h->merge[i].value);
        pddlLandmarkSeqFree(&h->merge[i].ldm_seq);
    }
    if (h->merge != NULL)
        BOR_FREE(h->merge);
    pddlStripsDel(h->strips);
}
