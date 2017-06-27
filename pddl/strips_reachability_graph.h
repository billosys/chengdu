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

#ifndef __PDDL_STRIPS_REACHABILITY_GRAPH_H__
#define __PDDL_STRIPS_REACHABILITY_GRAPH_H__

#include <pddl/strips.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_strips_reachability_node;

struct pddl_strips_reachability_edge {
    struct pddl_strips_reachability_node *from;
    struct pddl_strips_reachability_node *to;
    bor_list_t from_conn;
    bor_list_t to_conn;
};
typedef struct pddl_strips_reachability_edge pddl_strips_reachability_edge_t;

struct pddl_strips_reachability_node {
    int fact_id;
    int op_id;
    bor_list_t in;
    bor_list_t out;
};
typedef struct pddl_strips_reachability_node pddl_strips_reachability_node_t;

struct pddl_strips_reachability_fact_flags {
    int is_init;
    int is_goal;
    int removed;
};
typedef struct pddl_strips_reachability_fact_flags
                pddl_strips_reachability_fact_flags_t;

struct pddl_strips_reachability_graph {
    const pddl_strips_t *strips;
    pddl_strips_reachability_node_t root;
    pddl_strips_reachability_node_t *fact;
    pddl_strips_reachability_node_t *op;
    pddl_strips_reachability_fact_flags_t *fact_flags;
};
typedef struct pddl_strips_reachability_graph pddl_strips_reachability_graph_t;

void pddlStripsReachabilityGraphInit(pddl_strips_reachability_graph_t *rg,
                                     const pddl_strips_t *strips);
void pddlStripsReachabilityGraphFree(pddl_strips_reachability_graph_t *rg);
void pddlStripsReachabilityGraphRmOp(pddl_strips_reachability_graph_t *rg,
                                     int op_id,
                                     bor_iset_t *removed_facts,
                                     bor_iset_t *removed_ops);
void pddlStripsReachabilityGraphRmFact(pddl_strips_reachability_graph_t *rg,
                                       int fact_id,
                                       bor_iset_t *removed_facts,
                                       bor_iset_t *removed_ops);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_REACHABILITY_GRAPH_H__ */
