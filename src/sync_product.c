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

#include <boruvka/alloc.h>

#include "pddl/sync_product.h"
#include "err.h"

#define COST_INF (INT_MAX / 2)

size_t pddlSyncProductMaxNodes(const bor_iset_t *mgroups,
                               const pddl_strips_t *strips)
{
    size_t num_nodes;
    int mi;

    num_nodes = 1;
    BOR_ISET_FOR_EACH(mgroups, mi)
        num_nodes *= borISetSize(&strips->mgroup.mgroup[mi].fact);
    return num_nodes;
}

size_t pddlSyncProductRequiredMem(const bor_iset_t *mgroups,
                                  const pddl_strips_t *strips)
{
    size_t num_nodes = pddlSyncProductMaxNodes(mgroups, strips);
    size_t req_mem;

    req_mem  = num_nodes * sizeof(pddl_sync_product_node_t);
    req_mem += num_nodes * num_nodes * sizeof(pddl_sync_product_edge_t);
    req_mem += num_nodes * num_nodes * sizeof(int);
    req_mem += num_nodes * borISetSize(mgroups) * sizeof(int);
    return req_mem;
}

int pddlSyncProductCanFitInMem(const bor_iset_t *mgroups,
                               const pddl_strips_t *strips,
                               size_t mem)
{
    return pddlSyncProductRequiredMem(mgroups, strips) <= mem;
}

static void setUpMemory(pddl_sync_product_t *sprod)
{
    char *mem_fact, *mem_edge_next, *mem_edge_prev;
    size_t memsize;

    sprod->node = sprod->mem;

    mem_fact = (char *)(sprod->node + sprod->node_size);
    memsize = sizeof(int) * sprod->fact_size * sprod->node_size;
    mem_edge_next = mem_fact + memsize;
    memsize = sizeof(pddl_sync_product_edge_t)
                * (sprod->node_size * sprod->node_size);
    mem_edge_prev = mem_edge_next + memsize;

    bzero(sprod->mem, sprod->mem_size);
    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *n = sprod->node + i;
        n->fact = (int *)mem_fact + (size_t)sprod->fact_size * (size_t)i;

        n->next  = (pddl_sync_product_edge_t *)mem_edge_next;
        n->next += (size_t)sprod->node_size * (size_t)i;

        n->prev  = (int *)mem_edge_prev;
        n->prev += (size_t)sprod->node_size * (size_t)i;
    }
}

static void _initNodes(pddl_sync_product_t *sprod,
                       pddl_sync_product_node_t *node,
                       const bor_iset_t *mgroups,
                       const pddl_strips_t *strips,
                       int goal_size,
                       int init_size,
                       int mgroup_idx)
{
    // TODO: Use one set for the whole process
    BOR_ISET(facts);
    const pddl_mgroup_t *mgroup;
    int mgroups_size = borISetSize(mgroups);
    int inc, fact, is_mutex;

    mgroup = strips->mgroup.mgroup + borISetGet(mgroups, mgroup_idx);

    // TODO: this is computed again, and again
    inc = 1;
    for (int i = mgroup_idx + 1; i < mgroups_size; ++i)
        inc *= borISetSize(&strips->mgroup.mgroup[borISetGet(mgroups, i)].fact);

    BOR_ISET_FOR_EACH(&mgroup->fact, fact){
        borISetEmpty(&facts);
        for (int i = 0; i < mgroup_idx; ++i)
            borISetAdd(&facts, node->fact[i]);
        borISetAdd(&facts, fact);
        is_mutex = pddlMutexesIsMutex(&strips->mutex, &facts);

        for (int i = 0; i < inc; ++i){
            node[i].fact[mgroup_idx] = fact;
            node[i].is_mutex = is_mutex;
        }

        if (!is_mutex){
            if (mgroup_idx == mgroups_size - 1){
                int gsize = borISetIntersectionSize(&facts, &strips->goal);
                int isize = borISetIntersectionSize(&facts, &strips->init);
                node->is_goal = (gsize == goal_size);
                node->is_init = (isize == init_size);
                sprod->has_goal |= node->is_goal;

            }else{
                _initNodes(sprod, node, mgroups, strips, goal_size,
                           init_size, mgroup_idx + 1);
            }
        }
        node += inc;
    }

    borISetFree(&facts);
}

static void initNodes(pddl_sync_product_t *sprod,
                      const bor_iset_t *mgroups,
                      const pddl_strips_t *strips)
{
    BOR_ISET(facts);
    int mgi, goal_size, init_size;

    BOR_ISET_FOR_EACH(mgroups, mgi)
        borISetUnion(&facts, &strips->mgroup.mgroup[mgi].fact);
    goal_size = borISetIntersectionSize(&facts, &strips->goal);
    init_size = borISetIntersectionSize(&facts, &strips->init);
    if (goal_size == 0)
        goal_size = strips->fact.fact_size + 1;
    if (init_size == 0)
        init_size = strips->fact.fact_size + 1;

    _initNodes(sprod, sprod->node, mgroups, strips, goal_size, init_size, 0);
    borISetFree(&facts);
}

static void setAddOps(const pddl_sync_product_t *sprod,
                      const pddl_strips_cross_ref_t *cref,
                      const pddl_sync_product_node_t *from,
                      const pddl_sync_product_node_t *to,
                      const bor_iset_t *del_ops,
                      bor_iset_t *ops)
{
    borISetEmpty(ops);
    borISetUnion(ops, del_ops);
    for (int i = 0; i < sprod->fact_size; ++i){
        if (from->fact[i] != to->fact[i]){
            borISetIntersect(ops, &cref->fact[to->fact[i]].op_add);
            borISetIntersect(ops, &cref->fact[from->fact[i]].op_del);
        }else{
            borISetMinus(ops, &cref->fact[to->fact[i]].op_del);
        }
    }
}

static void initEdgesTo(pddl_sync_product_t *sprod,
                        const pddl_strips_t *strips,
                        const pddl_strips_cross_ref_t *cref,
                        int from_node_id,
                        const bor_iset_t *op_from)
{
    pddl_sync_product_node_t *from_node = sprod->node + from_node_id;
    BOR_ISET(ops);
    int opi;

    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;
        if (i == from_node_id || node->is_mutex)
            continue;

        setAddOps(sprod, cref, from_node, node, op_from, &ops);

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

    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;
        if (node->is_mutex)
            continue;
        for (int j = 0; j < sprod->node_size; ++j){
            node->next[j].cost = COST_INF;
            node->prev[j] = -1;
        }
    }


    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;
        if (node->is_mutex)
            continue;

        borISetEmpty(&ops);
        for (int j = 0; j < sprod->fact_size; ++j)
            borISetUnion(&ops, &cross_ref->fact[node->fact[j]].op_del);
        initEdgesTo(sprod, strips, cross_ref, i, &ops);
    }

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
        if (node->is_mutex)
            continue;

        minimizeNextEdges(node->next, sprod->node_size);
        minimizePrevEdges(node->prev, sprod->node_size);
    }

    BOR_ISET(costs);
    for (int i = 0; i < sprod->node_size; ++i){
        pddl_sync_product_node_t *node = sprod->node + i;
        if (node->is_mutex)
            continue;
        for (int j = 0; j < sprod->node_size; ++j){
            if (node->next[j].cost >= 0){
                borISetAdd(&costs, node->next[j].cost);
            }else{
                j -= node->next[j].cost + 1;
            }
        }
    }

    INFO2("CX");
    int c;
    BOR_ISET_FOR_EACH(&costs, c){
        INFO("Cost: %d", c);
    }
    borISetFree(&costs);
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

int pddlSyncProductInit(pddl_sync_product_t *sprod,
                        const bor_iset_t *mgroups,
                        const pddl_strips_t *strips,
                        const pddl_strips_cross_ref_t *cross_ref)
{
    bzero(sprod, sizeof(*sprod));
    sprod->mem_size = pddlSyncProductRequiredMem(mgroups, strips);
    sprod->mem = mmap(NULL, sprod->mem_size,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (sprod->mem == MAP_FAILED){
        ERR_RET(-1, "Allocation (using mmap) of %lu bytes failed!",
                (unsigned long)sprod->mem_size);
    }

    sprod->node_size = pddlSyncProductMaxNodes(mgroups, strips);
    sprod->fact_size = borISetSize(mgroups);

    // Set up pointers so that the whole synchronized product fits into the
    // preallocated memory
    setUpMemory(sprod);

    initNodes(sprod, mgroups, strips);
    initEdges(sprod, mgroups, strips, cross_ref);
    minimizeEdges(sprod);
    removeDeadEnds(sprod);

    return 0;
}

void pddlSyncProductFree(pddl_sync_product_t *sprod)
{
    if (sprod->mem != NULL)
        munmap(sprod->mem, sprod->mem_size);
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
    setAddOps(sprod, cref, nfrom, nto, &del_ops, &ops);

    borISetUnion(ops_out, &ops);
    borISetFree(&ops);
    borISetFree(&del_ops);
}
