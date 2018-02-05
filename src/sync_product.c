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

#include <sys/mman.h>
#include <limits.h>

#include <boruvka/pairheap.h>
#include <boruvka/alloc.h>

#include "pddl/sync_product.h"
#include "err.h"
#include "assert.h"

#define COST_INF (INT_MAX / 2)

static size_t maxNodes(const bor_iset_t *mgroups,
                       const pddl_strips_t *strips,
                       int num_landmarks)
{
    BOR_ISET(facts);
    BOR_ISET(mgroup_facts);
    int mgi;
    size_t max_nodes;

    max_nodes = 1;
    BOR_ISET_FOR_EACH(mgroups, mgi){
        const pddl_mgroup_t *mg = strips->mgroup.mgroup + mgi;
        borISetMinus2(&mgroup_facts, &mg->fact, &facts);
        max_nodes *= borISetSize(&mgroup_facts);
        borISetUnion(&facts, &mgroup_facts);
    }

    borISetFree(&facts);
    borISetFree(&mgroup_facts);

    max_nodes *= num_landmarks + 1;
    return max_nodes;
}

size_t pddlSyncProductMaxNodes(const bor_iset_t *mgroups,
                               const pddl_strips_t *strips)
{
    return maxNodes(mgroups, strips, 0);
}

static size_t requiredMemNodes(size_t num_nodes, size_t num_node_facts)
{
    size_t req_mem;

    req_mem  = num_nodes * sizeof(pddl_sync_product_node_t);
    req_mem += num_nodes * num_nodes * sizeof(pddl_sync_product_edge_t);
    req_mem += num_nodes * num_nodes * sizeof(int);
    req_mem += num_nodes * num_node_facts * sizeof(int);
    return req_mem;
}

static size_t requiredMem(const bor_iset_t *mgroups,
                          const pddl_strips_t *strips,
                          int num_landmarks)
{
    size_t num_nodes = maxNodes(mgroups, strips, num_landmarks);
    return requiredMemNodes(num_nodes, borISetSize(mgroups));
}

size_t pddlSyncProductRequiredMem(const bor_iset_t *mgroups,
                                  const pddl_strips_t *strips)
{
    return requiredMem(mgroups, strips, 0);
}

int pddlSyncProductCanFitInMem(const bor_iset_t *mgroups,
                               const pddl_strips_t *strips,
                               size_t mem)
{
    return pddlSyncProductLdmCanFitInMem(mgroups, strips, 0, mem);
}

int pddlSyncProductCanFitInMem2(size_t num_nodes, size_t num_node_facts,
                                size_t mem)
{
    return requiredMemNodes(num_nodes, num_node_facts) <= mem;
}

int pddlSyncProductLdmCanFitInMem(const bor_iset_t *mgroups,
                                  const pddl_strips_t *strips,
                                  int num_landmarks,
                                  size_t mem)
{
    return requiredMem(mgroups, strips, num_landmarks) <= mem;
}

static void setUpMemory(pddl_sync_product_t *sprod, size_t max_node_size)
{
    char *mem_fact, *mem_edge_next, *mem_edge_prev;
    size_t memsize;

    sprod->node = sprod->mem;

    mem_fact = (char *)(sprod->node + max_node_size);
    memsize = sizeof(int) * sprod->fact_size * max_node_size;
    mem_edge_next = mem_fact + memsize;
    memsize = sizeof(pddl_sync_product_edge_t)
                * (max_node_size * max_node_size);
    mem_edge_prev = mem_edge_next + memsize;

    bzero(sprod->mem, sprod->mem_size);
    for (int i = 0; i < max_node_size; ++i){
        pddl_sync_product_node_t *n = sprod->node + i;
        n->fact = (int *)mem_fact + (size_t)sprod->fact_size * (size_t)i;

        n->next  = (pddl_sync_product_edge_t *)mem_edge_next;
        n->next += (size_t)max_node_size * (size_t)i;

        n->prev  = (int *)mem_edge_prev;
        n->prev += (size_t)max_node_size * (size_t)i;
    }
}

static void _initNodesNextFact(pddl_sync_product_t *sprod,
                               const bor_iset_t *mgroups,
                               const pddl_strips_t *strips,
                               int goal_size,
                               int init_size,
                               int mgroup_idx,
                               bor_iset_t *facts,
                               int *fact_arr,
                               int fact);
static void _initNodes(pddl_sync_product_t *sprod,
                       const bor_iset_t *mgroups,
                       const pddl_strips_t *strips,
                       int goal_size,
                       int init_size,
                       int mgroup_idx,
                       bor_iset_t *facts,
                       int *fact_arr)
{
    const pddl_mgroup_t *mgroup;
    int fact = -1;

    mgroup = strips->mgroup.mgroup + borISetGet(mgroups, mgroup_idx);
    if (borISetIsDisjunct(facts, &mgroup->fact)){
        BOR_ISET_FOR_EACH(&mgroup->fact, fact){
            borISetAdd(facts, fact);
            if (!pddlMutexesIsMutex(&strips->mutex, facts)){
                _initNodesNextFact(sprod, mgroups, strips, goal_size,
                                   init_size, mgroup_idx, facts, fact_arr,
                                   fact);
            }
            borISetRm(facts, fact);
        }
    }else{
        BOR_ISET_FOR_EACH(facts, fact){
            if (borISetIn(fact, &mgroup->fact))
                break;
        }
        ASSERT(fact != -1);
        _initNodesNextFact(sprod, mgroups, strips, goal_size, init_size,
                           mgroup_idx, facts, fact_arr, fact);
    }
}

static void _initNodesNextFact(pddl_sync_product_t *sprod,
                               const bor_iset_t *mgroups,
                               const pddl_strips_t *strips,
                               int goal_size,
                               int init_size,
                               int mgroup_idx,
                               bor_iset_t *facts,
                               int *fact_arr,
                               int fact)
{
    int mgroups_size = borISetSize(mgroups);

    fact_arr[mgroup_idx] = fact;
    if (mgroup_idx == mgroups_size - 1){
        pddl_sync_product_node_t *node = sprod->node + sprod->node_size++;
        int gsize = borISetIntersectionSize(facts, &strips->goal);
        int isize = borISetIntersectionSize(facts, &strips->init);
        node->is_goal = (gsize == goal_size);
        node->is_init = (isize == init_size);
        sprod->has_goal |= node->is_goal;
        memcpy(node->fact, fact_arr, sizeof(int) * mgroups_size);
    }else{
        _initNodes(sprod, mgroups, strips, goal_size, init_size,
                   mgroup_idx + 1, facts, fact_arr);
    }
}

static void initNodes(pddl_sync_product_t *sprod,
                      const bor_iset_t *mgroups,
                      const pddl_strips_t *strips)
{
    BOR_ISET(facts);
    int *fact_arr;
    int mgi, goal_size, init_size;

    BOR_ISET_FOR_EACH(mgroups, mgi)
        borISetUnion(&facts, &strips->mgroup.mgroup[mgi].fact);
    goal_size = borISetIntersectionSize(&facts, &strips->goal);
    init_size = borISetIntersectionSize(&facts, &strips->init);
    if (goal_size == 0)
        goal_size = strips->fact.fact_size + 1;
    if (init_size == 0)
        init_size = strips->fact.fact_size + 1;

    fact_arr = BOR_ALLOC_ARR(int, borISetSize(mgroups));
    borISetEmpty(&facts);
    _initNodes(sprod, mgroups, strips, goal_size, init_size, 0,
               &facts, fact_arr);
    borISetFree(&facts);
    BOR_FREE(fact_arr);
}

static void initNodesLdm(pddl_sync_product_t *sprod,
                         const bor_iset_t *mgroups,
                         const pddl_strips_t *strips)
{
    pddl_sync_product_node_t *dst;

    for (int i = 0; i < sprod->node_size; ++i)
        sprod->node[i].ldm_level = 0;

    dst = sprod->node;
    for (int level = 0; level < sprod->ldm_seq.ldm_size; ++level){
        dst += sprod->node_size;
        for (int n = 0; n < sprod->node_size; ++n){
            dst[n].ldm_level = level + 1;
            dst[n].is_goal = sprod->node[n].is_goal;
            dst[n].is_init = sprod->node[n].is_init;
            memcpy(dst[n].fact, sprod->node[n].fact,
                   sizeof(int) * sprod->fact_size);
        }
    }

    sprod->node_size += sprod->node_size * sprod->ldm_seq.ldm_size;

    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *n = sprod->node + i;
        if (n->is_init && n->ldm_level != 0)
            n->is_init = 0;
        if (n->is_goal && n->ldm_level != sprod->ldm_seq.ldm_size)
            n->is_goal = 0;
    }
}

struct edge_ops_cache_pos {
    int fact_from;
    int fact_to;
    bor_iset_t ops;
};
typedef struct edge_ops_cache_pos edge_ops_cache_pos_t;

struct edge_ops_cache {
    edge_ops_cache_pos_t *pos;
    int pos_size;
    const pddl_sync_product_node_t *from_node;
    bor_iset_t init_ops;
};
typedef struct edge_ops_cache edge_ops_cache_t;

static void edgeOpsCacheInit(edge_ops_cache_t *cache,
                             const pddl_sync_product_t *sp)
{
    cache->pos_size = sp->fact_size - 1;
    cache->pos = BOR_CALLOC_ARR(edge_ops_cache_pos_t, cache->pos_size);
    for (int i = 0; i < cache->pos_size; ++i)
        cache->pos[i].fact_from = cache->pos[i].fact_to = -1;
    cache->from_node = NULL;
    borISetInit(&cache->init_ops);
}

static void edgeOpsCacheFree(edge_ops_cache_t *cache)
{
    for (int i = 0; i < cache->pos_size; ++i)
        borISetFree(&cache->pos[i].ops);
    if (cache->pos != NULL)
        BOR_FREE(cache->pos);
    borISetFree(&cache->init_ops);
}

static void edgeOpsCacheSetFromNode(edge_ops_cache_t *cache,
                                    const pddl_sync_product_node_t *from,
                                    const bor_iset_t *del_ops)
{
    cache->from_node = from;
    borISetSet(&cache->init_ops, del_ops);
    for (int i = 0; i < cache->pos_size; ++i)
        cache->pos[i].fact_from = cache->pos[i].fact_to = -1;
}

static void setAddOpsStep(const pddl_strips_cross_ref_t *cref,
                          int from_fact, int to_fact,
                          const bor_iset_t *src,
                          bor_iset_t *dst);

static void edgeOpsCacheSetPos(edge_ops_cache_t *cache,
                               int pos,
                               const pddl_strips_cross_ref_t *cref,
                               int from_fact, int to_fact)
{
    const bor_iset_t *src;

    if (pos == 0){
        src = &cache->init_ops;
    }else{
        src = &cache->pos[pos - 1].ops;
    }

    setAddOpsStep(cref, from_fact, to_fact, src, &cache->pos[pos].ops);
    cache->pos[pos].fact_from = from_fact;
    cache->pos[pos].fact_to = to_fact;
}

static void edgeOpsCache(edge_ops_cache_t *cache,
                         const pddl_strips_cross_ref_t *cref,
                         const pddl_sync_product_node_t *from,
                         const pddl_sync_product_node_t *to,
                         const bor_iset_t *del_ops,
                         bor_iset_t *ops)
{
    const bor_iset_t *src;
    int pos;

    // Initialize cache if it corresponds to a different from-node
    if (cache->from_node != from)
        edgeOpsCacheSetFromNode(cache, from, del_ops);

    // Find matching position steps
    for (pos = 0; pos < cache->pos_size; ++pos){
        if (cache->pos[pos].fact_from != from->fact[pos]
                || cache->pos[pos].fact_to != to->fact[pos])
            break;
    }

    // Fill-in the rest of the cache
    for (; pos < cache->pos_size; ++pos)
        edgeOpsCacheSetPos(cache, pos, cref, from->fact[pos], to->fact[pos]);

    // And finally, compute the output (which is not cached)
    if (pos == 0){
        src = &cache->init_ops;
    }else{
        src = &cache->pos[pos - 1].ops;
    }
    setAddOpsStep(cref, from->fact[pos], to->fact[pos], src, ops);
}

static void setAddOpsStep(const pddl_strips_cross_ref_t *cref,
                          int from_fact, int to_fact,
                          const bor_iset_t *src,
                          bor_iset_t *dst)
{
    if (from_fact != to_fact){
        if (src == dst){
            borISetIntersect(dst, &cref->fact[to_fact].op_add);
        }else{
            borISetIntersect2(dst, src, &cref->fact[to_fact].op_add);
        }
        borISetIntersect(dst, &cref->fact[from_fact].op_del);
    }else{
        if (src == dst){
            borISetMinus(dst, &cref->fact[to_fact].op_del);
        }else{
            borISetMinus2(dst, src, &cref->fact[to_fact].op_del);
        }
        borISetMinus(dst, &cref->fact[to_fact].op_pre_mutex);
    }
}

static void setAddOps(const pddl_sync_product_t *sp,
                      const pddl_strips_cross_ref_t *cref,
                      const pddl_sync_product_node_t *from,
                      const pddl_sync_product_node_t *to,
                      const bor_iset_t *del_ops,
                      bor_iset_t *ops,
                      edge_ops_cache_t *cache)
{
    if (from->ldm_level == to->ldm_level && cache != NULL){
        edgeOpsCache(cache, cref, from, to, del_ops, ops);
        return;

    }else if (from->ldm_level == to->ldm_level){
        borISetSet(ops, del_ops);

        /*
        if (sp->ldm_seq.ldm_size > 0
                && from->ldm_level < sp->ldm_seq.ldm_size)
            borISetMinus(ops, sp->ldm_seq.ldm + from->ldm_level);
        */
    }else{
        // Allow edges only from lower to higher levels
        if (to->ldm_level < from->ldm_level)
            return;
        ASSERT(from->ldm_level < sp->ldm_seq.ldm_size);

        if (memcmp(from->fact, to->fact, sizeof(int) * sp->fact_size) == 0){
            // If the nodes differ only in the level use only the landmark
            // operators that will be later pruned to those that does not
            // touch any fact and are applicable in the node
            borISetSet(ops, sp->ldm_seq.ldm + from->ldm_level);

        }else{
            // If the nodes differ restrict the operator to only those that
            // are part of the respective landmark.
            borISetIntersect2(ops, del_ops, sp->ldm_seq.ldm + from->ldm_level);
        }

        // The edges across more than one level must correspond to the
        // operators that would hit all intermediate landmarks.
        for (int i = from->ldm_level + 1;
                i < to->ldm_level && borISetSize(ops) > 0; ++i){
            borISetIntersect(ops, sp->ldm_seq.ldm + i);
        }
    }

    for (int i = 0; i < sp->fact_size && borISetSize(ops) > 0; ++i)
        setAddOpsStep(cref, from->fact[i], to->fact[i], ops, ops);
}

static void initEdgesTo(pddl_sync_product_t *sprod,
                        const pddl_strips_t *strips,
                        const pddl_strips_cross_ref_t *cref,
                        int from_node_id,
                        const bor_iset_t *op_from,
                        edge_ops_cache_t *cache)
{
    pddl_sync_product_node_t *from_node = sprod->node + from_node_id;
    BOR_ISET(ops);
    int opi;

    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;
        if (i == from_node_id)
            continue;

        setAddOps(sprod, cref, from_node, node, op_from, &ops, cache);

        BOR_ISET_FOR_EACH(&ops, opi){
            pddl_sync_product_edge_t *next = from_node->next + i;
            int op_cost = strips->op.op[opi]->cost;
            next->cost = BOR_MIN(next->cost, op_cost);
            node->prev[from_node_id] = 1;
        }
    }

    borISetFree(&ops);
}

static void initEdges(pddl_sync_product_t *sprod,
                      const bor_iset_t *mgroups,
                      const pddl_strips_t *strips,
                      const pddl_strips_cross_ref_t *cross_ref)
{
    BOR_ISET(ops);
    edge_ops_cache_t cache;

    edgeOpsCacheInit(&cache, sprod);
    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;
        for (int j = 0; j < sprod->node_size; ++j){
            node->next[j].cost = COST_INF;
            node->prev[j] = -1;
        }
    }


    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;

        borISetEmpty(&ops);
        for (int j = 0; j < sprod->fact_size; ++j)
            borISetUnion(&ops, &cross_ref->fact[node->fact[j]].op_del);
        initEdgesTo(sprod, strips, cross_ref, i, &ops, &cache);
    }

    edgeOpsCacheFree(&cache);
    borISetFree(&ops);
}

static void minimizeNextEdges(pddl_sync_product_edge_t *edge, int node_size)
{
    for (int j = node_size - 1, p = 0; j >= 0; --j){
        if (edge[j].cost != COST_INF){
            p = 0;
        }else{
            edge[j].cost = -1;
            if (p)
                edge[j].cost += edge[j + 1].cost;
            p = 1;
        }
    }
}

static void minimizePrevEdges(int *edge, int node_size)
{
    for (int j = node_size - 1, p = 0; j >= 0; --j){
        if (edge[j] > 0){
            p = 0;
        }else{
            edge[j] = -1;
            if (p)
                edge[j] += edge[j + 1];
            p = 1;
        }
    }
}

static void minimizeEdges(pddl_sync_product_t *sprod)
{
    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;

        minimizeNextEdges(node->next, sprod->node_size);
        minimizePrevEdges(node->prev, sprod->node_size);
    }
}

static int nodeHasNoOutgoingEdges(const pddl_sync_product_t *sprod,
                                  const pddl_sync_product_node_t *n)
{
    return n->next[0].cost == -1 * sprod->node_size;
}

static void removeDeadEnds(pddl_sync_product_t *sprod)
{
    if (!sprod->has_goal)
        return;

    BOR_ISET(dead_ends);
    for (int i = 0; i < sprod->node_size; ++i){
        const pddl_sync_product_node_t *n = sprod->node + i;
        if (nodeHasNoOutgoingEdges(sprod, n) && !n->is_goal){
            // TODO
            fprintf(stderr, "TODO: DEAD END\n");
        }
    }
    borISetFree(&dead_ends);
}

int syncProductInit(pddl_sync_product_t *sprod,
                    const pddl_strips_t *strips,
                    const pddl_strips_cross_ref_t *cross_ref,
                    const bor_iset_t *mgroups,
                    const pddl_landmark_seq_t *lseq,
                    int max_nodes)
{
    bzero(sprod, sizeof(*sprod));

    pddlLandmarkSeqInit(&sprod->ldm_seq);
    if (lseq != NULL)
        pddlLandmarkSeqCopy(&sprod->ldm_seq, lseq);

    if (max_nodes > 0){
        if (sprod->ldm_seq.ldm_size > 0)
            max_nodes *= sprod->ldm_seq.ldm_size;
        sprod->mem_size = requiredMemNodes(max_nodes, borISetSize(mgroups));
    }else{
        sprod->mem_size = requiredMem(mgroups, strips, sprod->ldm_seq.ldm_size);
    }
    sprod->mem = mmap(NULL, sprod->mem_size,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (sprod->mem == MAP_FAILED){
        ERR_RET(-1, "Allocation (using mmap) of %lu bytes failed!",
                (unsigned long)sprod->mem_size);
    }

    sprod->node_size = 0;
    sprod->fact_size = borISetSize(mgroups);

    // Set up pointers so that the whole synchronized product fits into the
    // preallocated memory
    if (max_nodes > 0){
        setUpMemory(sprod, max_nodes);
    }else{
        setUpMemory(sprod, maxNodes(mgroups, strips, sprod->ldm_seq.ldm_size));
    }

    initNodes(sprod, mgroups, strips);
    if (sprod->ldm_seq.ldm_size > 0)
        initNodesLdm(sprod, mgroups, strips);
    initEdges(sprod, mgroups, strips, cross_ref);
    minimizeEdges(sprod);
    //removeDeadEnds(sprod);

    return 0;
}

int pddlSyncProductInit(pddl_sync_product_t *sprod,
                        const bor_iset_t *mgroups,
                        const pddl_strips_t *strips,
                        const pddl_strips_cross_ref_t *cross_ref)
{
    return syncProductInit(sprod, strips, cross_ref, mgroups, NULL, 0);
}

int pddlSyncProductInitMaxNodes(pddl_sync_product_t *sprod,
                                const bor_iset_t *mgroups,
                                const pddl_strips_t *strips,
                                const pddl_strips_cross_ref_t *cross_ref,
                                int max_nodes)
{
    return syncProductInit(sprod, strips, cross_ref, mgroups, NULL, max_nodes);
}

int pddlSyncProductInitLdm(pddl_sync_product_t *sprod,
                           const pddl_strips_t *strips,
                           const pddl_strips_cross_ref_t *cross_ref,
                           const bor_iset_t *mgroups,
                           const pddl_landmark_seq_t *lseq)
{
    return syncProductInit(sprod, strips, cross_ref, mgroups, lseq, 0);
}

void pddlSyncProductFree(pddl_sync_product_t *sprod)
{
    if (sprod->mem != NULL)
        munmap(sprod->mem, sprod->mem_size);
    pddlLandmarkSeqFree(&sprod->ldm_seq);
}

void pddlSyncProductEdgeOps(const pddl_sync_product_t *sprod,
                            const pddl_strips_cross_ref_t *cref,
                            int from_node, int to_node,
                            bor_iset_t *ops_out)
{
    const pddl_sync_product_node_t *nfrom = sprod->node + from_node;
    const pddl_sync_product_node_t *nto = sprod->node + to_node;
    BOR_ISET(ops);
    BOR_ISET(del_ops);

    for (int j = 0; j < sprod->fact_size; ++j)
        borISetUnion(&del_ops, &cref->fact[nfrom->fact[j]].op_del);
    setAddOps(sprod, cref, nfrom, nto, &del_ops, &ops, NULL);

    borISetUnion(ops_out, &ops);
    borISetFree(&ops);
    borISetFree(&del_ops);
}

int pddlSyncProductInitNode(const pddl_sync_product_t *sprod)
{
    for (int i = 0; i < sprod->node_size; ++i){
        if (sprod->node[i].is_init)
            return i;
    }
    return -1;
}


struct dij_node {
    int node_id;
    bor_pairheap_node_t heap;
};
typedef struct dij_node dij_node_t;

static int dijLT(const bor_pairheap_node_t *_n1,
                 const bor_pairheap_node_t *_n2,
                 void *data)
{
    bor_iarr_t *dist = data;
    const dij_node_t *n1 = bor_container_of(_n1, dij_node_t, heap);
    const dij_node_t *n2 = bor_container_of(_n2, dij_node_t, heap);
    return borIArrGet(dist, n1->node_id) < borIArrGet(dist, n2->node_id);
}

void pddlSyncProductGoalDistance(const pddl_sync_product_t *sprod,
                                 bor_iarr_t *dist)
{
    bor_pairheap_t *heap;
    dij_node_t *dij_node;

    // Allocate output array
    borIArrResize(dist, sprod->node_size);
    if (!sprod->has_goal){
        for (int i = 0; i < sprod->node_size; ++i)
            borIArrSet(dist, i, -1);
        return;
    }

    // Allocate and initialize structures for dijkstra algorithm
    dij_node = BOR_ALLOC_ARR(dij_node_t, sprod->node_size);
    heap = borPairHeapNew(dijLT, dist);
    for (int i = 0; i < sprod->node_size; ++i){
        const pddl_sync_product_node_t *node = sprod->node + i;
        if (node->is_goal){
            borIArrSet(dist, i, 0);
        }else{
            borIArrSet(dist, i, COST_INF);
        }
        dij_node[i].node_id = i;
        borPairHeapAdd(heap, &dij_node[i].heap);
    }

    // Run dijkstra algorithm
    while (!borPairHeapEmpty(heap)){
        bor_pairheap_node_t *pn = borPairHeapExtractMin(heap);
        dij_node_t *n = bor_container_of(pn, dij_node_t, heap);
        PDDL_SYNC_PRODUCT_FOR_EACH_PREV(sprod, n->node_id, next_id){
            int d = borIArrGet(dist, n->node_id)
                        + sprod->node[next_id].next[n->node_id].cost;
            if (d < borIArrGet(dist, next_id)){
                borIArrSet(dist, next_id, d);
                borPairHeapUpdate(heap, &dij_node[next_id].heap);
            }
        }
    }

    // Rewrite COST_INF to -1
    for (int i = 0; i < sprod->node_size; ++i){
        if (borIArrGet(dist, i) == COST_INF)
            borIArrSet(dist, i, -1);
    }

    BOR_FREE(dij_node);
    borPairHeapDel(heap);
}


static void ldmInit(pddl_sync_product_t *sprod,
                    bor_iarr_t *goal_zone_queue)
{
    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;
        node->is_goal_zone = 0;
        if (node->is_goal){
            borIArrAdd(goal_zone_queue, i);
            node->is_goal_zone = 1;
        }
        PDDL_SYNC_PRODUCT_FOR_EACH_NEXT(sprod, node, next_id)
            node->next[next_id].cur_cost = node->next[next_id].cost;
    }
}

static void ldmMarkGoalZone(pddl_sync_product_t *sprod,
                            bor_iarr_t *queue)
{
    for (int idx = 0; idx < borIArrSize(queue); ++idx){
        int node_id = borIArrGet(queue, idx);

        PDDL_SYNC_PRODUCT_FOR_EACH_PREV(sprod, node_id, prev_id){
            if (sprod->node[prev_id].is_goal_zone)
                continue;

            if (sprod->node[prev_id].next[node_id].cur_cost == 0){
                borIArrAdd(queue, prev_id);
                sprod->node[prev_id].is_goal_zone = 1;
            }
        }
    }
}

static void ldmFindCut(pddl_sync_product_t *sprod,
                       const pddl_strips_cross_ref_t *cref,
                       int init_node,
                       bor_iarr_t *queue,
                       bor_iset_t *cut_op,
                       bor_iset_t *cut_node,
                       int *cut_cost)
{
    for (int i = 0; i < sprod->node_size; ++i)
        sprod->node[i].is_init_zone = 0;

    borISetEmpty(cut_op);
    borISetEmpty(cut_node);
    *cut_cost = COST_INF;

    borIArrEmpty(queue);
    borIArrAdd(queue, init_node);
    sprod->node[init_node].is_init_zone = 1;

    for (int idx = 0; idx < borIArrSize(queue); ++idx){
        int node_id = borIArrGet(queue, idx);
        pddl_sync_product_node_t *node = sprod->node + node_id;

        PDDL_SYNC_PRODUCT_FOR_EACH_NEXT(sprod, node, next_id){
            pddl_sync_product_node_t *next = sprod->node + next_id;
            if (next->is_init_zone)
                continue;

            if (next->is_goal_zone){
                pddlSyncProductEdgeOps(sprod, cref, node_id, next_id, cut_op);
                borISetAdd(cut_node, node_id);
                *cut_cost = BOR_MIN(*cut_cost, node->next[next_id].cur_cost);
            }else{
                borIArrAdd(queue, next_id);
                next->is_init_zone = 1;
            }
        }
    }
}

static void ldmApplyCut(pddl_sync_product_t *sprod,
                        const bor_iset_t *cut_nodes,
                        int cut_cost)
{
    int node_id;

    BOR_ISET_FOR_EACH(cut_nodes, node_id){
        pddl_sync_product_node_t *node = sprod->node + node_id;
        PDDL_SYNC_PRODUCT_FOR_EACH_NEXT(sprod, node, next_id){
            if (sprod->node[next_id].is_goal_zone)
                node->next[next_id].cur_cost -= cut_cost;
        }
    }
}

static void ldm(pddl_sync_product_t *sprod,
                const pddl_strips_cross_ref_t *cref,
                int init_node_id,
                pddl_landmarks_t *ldms,
                pddl_landmark_seq_t *lseq,
                bor_iset_t *ldm_union,
                int *ldm_cost)
{
    BOR_IARR(goal_zone_queue);
    BOR_IARR(init_zone_queue);
    BOR_ISET(cut_op);
    BOR_ISET(cut_node);
    pddl_sync_product_node_t *init_node = sprod->node + init_node_id;
    int cut_cost;

    ldmInit(sprod, &goal_zone_queue);
    ldmMarkGoalZone(sprod, &goal_zone_queue);
    while (!init_node->is_goal_zone){
        ldmFindCut(sprod, cref, init_node_id, &init_zone_queue,
                   &cut_op, &cut_node, &cut_cost);

        // The initial node can be disconnected from the goal nodes
        if (borISetSize(&cut_node) == 0)
            break;

        if (ldms != NULL)
            pddlLandmarksAdd(ldms, &cut_op);
        if (lseq != NULL)
            pddlLandmarkSeqPrepend(lseq, &cut_op);
        if (ldm_union != NULL)
            borISetUnion(ldm_union, &cut_op);
        if (ldm_cost != NULL)
            *ldm_cost += cut_cost;

        ldmApplyCut(sprod, &cut_node, cut_cost);
        ldmMarkGoalZone(sprod, &goal_zone_queue);
    }

    borISetFree(&cut_op);
    borISetFree(&cut_node);
    borIArrFree(&goal_zone_queue);
    borIArrFree(&init_zone_queue);
}

int pddlSyncProductFindLandmarks(pddl_sync_product_t *sprod,
                                 const pddl_strips_cross_ref_t *cref,
                                 int init_node,
                                 pddl_landmarks_t *ldms,
                                 pddl_landmark_seq_t *lseq,
                                 bor_iset_t *ldm_union,
                                 int *ldm_cost)
{
    if (!sprod->has_goal)
        ERR_RET2(-1, "Synchronized product does not contain any goal node.");
    if (ldms == NULL && lseq == NULL && ldm_union == NULL && ldm_cost == NULL){
        ERR_RET2(-1, "No output specified.");
    }
    if (sprod->node[init_node].is_goal)
        return 0;

    if (lseq != NULL)
        pddlLandmarkSeqEmpty(lseq);
    if (ldm_cost != NULL)
        *ldm_cost = 0;
    ldm(sprod, cref, init_node, ldms, lseq, ldm_union, ldm_cost);
    return 0;
}
