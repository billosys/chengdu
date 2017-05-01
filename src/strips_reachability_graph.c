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

#include "pddl/strips_reachability_graph.h"

static void delEdges(pddl_strips_reachability_node_t *node)
{
    pddl_strips_reachability_edge_t *edge;
    bor_list_t *item;

    while (!borListEmpty(&node->in)){
        item = borListNext(&node->in);
        borListDel(item);
        edge = BOR_LIST_ENTRY(item, pddl_strips_reachability_edge_t, to_conn);
        borListDel(&edge->from_conn);
        BOR_FREE(edge);
    }

    while (!borListEmpty(&node->out)){
        item = borListNext(&node->out);
        borListDel(item);
        edge = BOR_LIST_ENTRY(item, pddl_strips_reachability_edge_t, from_conn);
        borListDel(&edge->to_conn);
        BOR_FREE(edge);
    }
}

static void addEdge(pddl_strips_reachability_graph_t *rg,
                    pddl_strips_reachability_node_t *from_node,
                    pddl_strips_reachability_node_t *to_node)
{
    pddl_strips_reachability_edge_t *edge;

    edge = BOR_ALLOC(pddl_strips_reachability_edge_t);
    edge->from = from_node;
    edge->to = to_node;
    borListAppend(&from_node->out, &edge->from_conn);
    borListAppend(&to_node->out, &edge->to_conn);
}

void pddlStripsReachabilityGraphInit(pddl_strips_reachability_graph_t *rg,
                                     const pddl_strips_t *strips)
{
    bzero(rg, sizeof(*rg));
    rg->strips = strips;

    rg->root.fact_id = -1;
    rg->root.op_id = -1;
    borListInit(&rg->root.in);
    borListInit(&rg->root.out);

    rg->fact_flags = BOR_CALLOC_ARR(pddl_strips_reachability_fact_flags_t,
                                    strips->fact.fact_size);
    rg->fact = BOR_CALLOC_ARR(pddl_strips_reachability_node_t,
                              strips->fact.fact_size);
    for (int i = 0; i < strips->fact.fact_size; ++i){
        rg->fact[i].fact_id = i;
        rg->fact[i].op_id = -1;
        borListInit(&rg->fact[i].in);
        borListInit(&rg->fact[i].out);
    }

    rg->op = BOR_CALLOC_ARR(pddl_strips_reachability_node_t,
                            strips->op.op_size);
    for (int i = 0; i < strips->op.op_size; ++i){
        rg->fact[i].fact_id = -1;
        rg->op[i].op_id = i;
        borListInit(&rg->op[i].in);
        borListInit(&rg->op[i].out);
    }

    for (int i = 0; i < strips->init.size; ++i){
        addEdge(rg, &rg->root, rg->fact + strips->init.fact[i]);
        rg->fact_flags[strips->init.fact[i]].is_init = 1;
    }
    for (int i = 0; i < strips->goal.size; ++i)
        rg->fact_flags[strips->goal.fact[i]].is_goal = 1;

    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        for (int j = 0; j < op->pre.size; ++j)
            addEdge(rg, &rg->fact[op->pre.fact[j]], &rg->op[i]);
        for (int j = 0; j < op->add_eff.size; ++j)
            addEdge(rg, &rg->op[i], &rg->fact[op->add_eff.fact[j]]);
    }
}

void pddlStripsReachabilityGraphFree(pddl_strips_reachability_graph_t *rg)
{
    delEdges(&rg->root);
    for (int i = 0; i < rg->strips->fact.fact_size; ++i)
        delEdges(rg->fact + i);
    for (int i = 0; i < rg->strips->op.op_size; ++i)
        delEdges(rg->op + i);

    if (rg->fact != NULL)
        BOR_FREE(rg->fact);
    if (rg->op != NULL)
        BOR_FREE(rg->op);
    if (rg->fact_flags != NULL)
        BOR_FREE(rg->fact_flags);
}

void pddlStripsReachabilityGraphRmOp(pddl_strips_reachability_graph_t *rg,
                                     int op_id,
                                     pddl_fact_id_arr_t *removed_facts,
                                     pddl_fact_id_arr_t *removed_ops)
{
    pddl_strips_reachability_node_t *op_node = rg->op + op_id;
    pddl_strips_reachability_node_t *fact_node;
    pddl_strips_reachability_edge_t *edge;
    bor_list_t *litem;
    pddl_fact_id_arr_t rm_facts;

    pddlFactIdArrInit(&rm_facts);

    // Remove incoming edges
    while (!borListEmpty(&op_node->in)){
        litem = borListNext(&op_node->in);
        borListDel(litem);
        edge = BOR_LIST_ENTRY(litem, pddl_strips_reachability_edge_t, to_conn);
        borListDel(&edge->from_conn);
        BOR_FREE(edge);
    }

    // Remove out edges
    while (!borListEmpty(&op_node->out)){
        litem = borListNext(&op_node->out);
        borListDel(litem);
        edge = BOR_LIST_ENTRY(litem, pddl_strips_reachability_edge_t,
                              from_conn);
        fact_node = edge->to;
        borListDel(&edge->to_conn);
        BOR_FREE(edge);

        // Determine whether the next fact node has no in edges and if so
        // schedule it for removal.
        if (borListEmpty(&fact_node->in))
            pddlFactIdArrAdd(&rm_facts, fact_node->fact_id);
    }

    pddlFactIdArrAdd(removed_ops, op_id);

    // Remove facts with no incoming edges
    for (int i = 0; i < rm_facts.size; ++i)
        pddlStripsReachabilityGraphRmFact(rg, rm_facts.fact[i],
                                          removed_facts, removed_ops);

    pddlFactIdArrFree(&rm_facts);
}

void pddlStripsReachabilityGraphRmFact(pddl_strips_reachability_graph_t *rg,
                                       int fact_id,
                                       pddl_fact_id_arr_t *removed_facts,
                                       pddl_fact_id_arr_t *removed_ops)
{
    pddl_strips_reachability_node_t *fact_node = rg->fact + fact_id;
    pddl_strips_reachability_edge_t *edge;
    bor_list_t *litem;
    pddl_fact_id_arr_t rm;

    pddlFactIdArrInit(&rm);

    while (!borListEmpty(&fact_node->in)){
        litem = borListNext(&fact_node->in);
        borListDel(litem);
        edge = BOR_LIST_ENTRY(litem, pddl_strips_reachability_edge_t, to_conn);
        borListDel(&edge->from_conn);
        BOR_FREE(edge);
    }

    while (!borListEmpty(&fact_node->out)){
        litem = borListNext(&fact_node->out);
        borListDel(litem);
        edge = BOR_LIST_ENTRY(litem, pddl_strips_reachability_edge_t,
                              from_conn);
        pddlFactIdArrAdd(&rm, edge->to->op_id);
        borListDel(&edge->to_conn);
        BOR_FREE(edge);
    }

    pddlFactIdArrAdd(removed_facts, fact_id);

    for (int i = 0; i < rm.size; ++i)
        pddlStripsReachabilityGraphRmOp(rg, rm.fact[i],
                                        removed_facts, removed_ops);

    pddlFactIdArrFree(&rm);
}
