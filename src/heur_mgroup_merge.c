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
#include "pddl/heur_mgroup_merge.h"
#include "pddl/mgroup_op_bipartite_graph.h"
#include "pddl/sync_product.h"
#include "pddl/util.h"
#include "err.h"

#define SCALE_LIMIT 10000
#define SCALE_HARD_LIMIT 100000

struct merge_candidate {
    int mg[2];
    int size;
};
typedef struct merge_candidate merge_candidate_t;

struct merge_candidates {
    merge_candidate_t *candidate;
    int candidate_size;
    int *merge_map;
};
typedef struct merge_candidates merge_candidates_t;

// TODO
const size_t MAX_MEM = 4UL * 1024UL * 1024UL * 1024UL;

static int canMergeNodes(const pddl_mgroup_op_bipartite_graph_t *g,
                         int n1, int n2,
                         const pddl_strips_t *strips,
                         size_t max_mem)
{
    BOR_ISET(mgs);
    int can_fit;

    borISetUnion(&mgs, &g->mgroup[n1].mgroup);
    borISetUnion(&mgs, &g->mgroup[n2].mgroup);
    can_fit = pddlSyncProductCanFitInMem(&mgs, strips, max_mem);
    borISetFree(&mgs);
    return can_fit;
}

static int canMergeMGroups(const bor_iset_t *mgs1, const bor_iset_t *mgs2,
                           const pddl_strips_t *strips,
                           size_t max_mem)
{
    BOR_ISET(mgs);
    int can_fit;

    borISetUnion(&mgs, mgs1);
    borISetUnion(&mgs, mgs2);
    can_fit = pddlSyncProductCanFitInMem(&mgs, strips, max_mem);
    borISetFree(&mgs);
    return can_fit;
}


static void makeMergeMap(const pddl_mgroup_op_bipartite_graph_t *g,
                         int *map)
{
    bzero(map, sizeof(int) * g->mgroup_size * g->mgroup_size);
    for (int i = 0; i < g->op_size; ++i){
        const pddl_mgroup_op_bipartite_graph_op_t *op = g->op + i;
        for (int j = 0; j < borISetSize(&op->mgroup); ++j){
            int mg1 = borISetGet(&op->mgroup, j);
            for (int k = j + 1; k < borISetSize(&op->mgroup); ++k){
                int mg2 = borISetGet(&op->mgroup, k);
                map[mg1 * g->mgroup_size + mg2] += borISetSize(&op->op);
                map[mg2 * g->mgroup_size + mg1] += borISetSize(&op->op);
            }
        }
    }
}

static void mergeCandidatesInit(merge_candidates_t *cand,
                                const pddl_mgroup_op_bipartite_graph_t *g)
{
    cand->candidate = BOR_ALLOC_ARR(merge_candidate_t,
                                    g->mgroup_size * g->mgroup_size);
    cand->merge_map = BOR_ALLOC_ARR(int, g->mgroup_size * g->mgroup_size);
}

static void mergeCandidatesFree(merge_candidates_t *cand)
{
    if (cand->candidate != NULL)
        BOR_FREE(cand->candidate);
    if (cand->merge_map != NULL)
        BOR_FREE(cand->merge_map);
}

static int mergeCandidateCmp(const void *a, const void *b, void *_)
{
    const merge_candidate_t *c1 = a;
    const merge_candidate_t *c2 = b;
    return c2->size - c1->size;
}

static void mergeCandidatesReset(merge_candidates_t *cand,
                                 const pddl_mgroup_op_bipartite_graph_t *g)
{
    makeMergeMap(g, cand->merge_map);
    cand->candidate_size = 0;
    for (int i = 0; i < g->mgroup_size; ++i){
        for (int j = i + 1; j < g->mgroup_size; ++j){
            int size = cand->merge_map[i * g->mgroup_size + j];
            if (size == 0)
                continue;
            merge_candidate_t *c = cand->candidate + cand->candidate_size++;
            c->mg[0] = i;
            c->mg[1] = j;
            c->size = size;
        }
    }
    borSort(cand->candidate, cand->candidate_size, sizeof(merge_candidate_t),
            mergeCandidateCmp, NULL);
}

static void findMerges(pddl_heur_mgroup_merge_t *h,
                       const pddl_strips_t *strips,
                       const pddl_strips_cross_ref_t *cref)
{
    pddl_mgroup_op_bipartite_graph_t graph;
    merge_candidates_t cands;
    int change;

    pddlMGroupOpBipartiteGraphInit(&graph, strips, cref);
    mergeCandidatesInit(&cands, &graph);

    // Merge mgroup nodes so that it greedily minimizes number of common
    // operators
    change = 1;
    while (change){
        change = 0;

        pddlMGroupOpBipartiteGraphMinimize(&graph);
        mergeCandidatesReset(&cands, &graph);
        for (int i = 0; i < cands.candidate_size; ++i){
            const merge_candidate_t *c = cands.candidate + i;
            int m1 = c->mg[0];
            int m2 = c->mg[1];
            if (!graph.mgroup[m1].is_goal && !graph.mgroup[m2].is_goal)
                continue;
            if (canMergeNodes(&graph, m1, m2, strips, MAX_MEM)){
                pddlMGroupOpBipartiteGraphMerge(&graph, m1, m2);
                change = 1;
                break;
            }
        }
    }

    // Prepare merges from the mgroup nodes of the bipartite graph
    h->merge = BOR_CALLOC_ARR(pddl_heur_mgroup_merge_merge_t,
                              graph.mgroup_size);
    h->merge_size = 0;
    for (int i = 0; i < graph.mgroup_size; ++i){
        const pddl_mgroup_op_bipartite_graph_mgroup_t *m = graph.mgroup + i;
        if (m->is_goal && borISetSize(&m->mgroup) > 0){
            borISetUnion(&h->merge[h->merge_size].mgroup, &m->mgroup);
            ++h->merge_size;
        }
    }

    // Try to extend the merges with mgroup nodes that are not goal nodes
    for (int i = 0; i < graph.mgroup_size; ++i){
        const pddl_mgroup_op_bipartite_graph_mgroup_t *m = graph.mgroup + i;
        if (!m->is_goal && borISetSize(&m->mgroup) > 0){
            for (int j = 0; j < h->merge_size; ++j){
                if (canMergeMGroups(&h->merge[j].mgroup, &m->mgroup,
                                    strips, MAX_MEM)){
                    borISetUnion(&h->merge[j].mgroup, &m->mgroup);
                }
            }
        }
    }

    /*
    pddlMGroupOpBipartiteGraphMinimize(&graph);
    mergeCandidatesReset(&cands, &graph);
    for (int i = 0; i < cands.candidate_size; ++i){
        fprintf(stderr, "%d: %d + %d -- %d\n",
                i, cands.candidate[i].mg[0],
                cands.candidate[i].mg[1],
                cands.candidate[i].size);
    }

    pddlMGroupOpBipartiteGraphMinimize(&graph);
    pddlMGroupOpBipartiteGraphPrint(&graph, stderr);
    */

    for (int i = 0; i < h->merge_size; ++i){
        int m;
        fprintf(stderr, "Merge:");
        BOR_ISET_FOR_EACH(&h->merge[i].mgroup, m)
            fprintf(stderr, " %d", m);
        fprintf(stderr, "\n");
    }

    mergeCandidatesFree(&cands);
    pddlMGroupOpBipartiteGraphFree(&graph);
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
    INFO("Operator cost scaling factor: %d", scale);

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

    pddlSyncProductInit(&sp, &merge->mgroup, h->strips, cref);
    INFO("Sync Product computed, nodes: %d", sp.node_size);

    pddlSyncProductGoalDistance(&sp, &dist);
    for (int i = 0; i < sp.node_size; ++i){
        const pddl_sync_product_node_t *n = sp.node + i;
        if (n->is_mutex)
            continue;
        addValue(merge, &sp, n, borIArrGet(&dist, i));
    }
    INFO("Goal Distances computed, values: %d", merge->value_size);

    pddlSyncProductFree(&sp);
    borIArrFree(&dist);
}

void pddlHeurMGroupMergeInit(pddl_heur_mgroup_merge_t *h,
                             const pddl_strips_t *_strips)
{
    pddl_strips_t *strips = pddlStripsClone(_strips);
    pddl_strips_cross_ref_t cref;

    pddlStripsCompleteMGroups(strips);
    pddlStripsMakeExactlyOneMGroups(strips);
    pddlStripsCrossRefInit(&cref, strips);

    bzero(h, sizeof(*h));
    h->strips = strips;
    h->op_cost_scale = 1;

    findMerges(h, strips, &cref);
    costPart(h, &cref);
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
    }
    if (h->merge != NULL)
        BOR_FREE(h->merge);
    pddlStripsDel(h->strips);
}
