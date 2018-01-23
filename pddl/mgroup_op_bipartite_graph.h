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

#ifndef __PDDL_MGROUP_OP_BIPARTITE_GRAPH_H__
#define __PDDL_MGROUP_OP_BIPARTITE_GRAPH_H__

#include <boruvka/iset.h>

#include <pddl/common.h>
#include <pddl/strips.h>
#include <pddl/strips_cross_ref.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_mgroup_op_bipartite_graph_op {
    bor_iset_t mgroup; /*!< Connections to mgroup nodes */
    bor_iset_t op; /*!< Operators this node corresponds to */
};
typedef struct pddl_mgroup_op_bipartite_graph_op
    pddl_mgroup_op_bipartite_graph_op_t;

struct pddl_mgroup_op_bipartite_graph_mgroup {
    bor_iset_t mgroup; /*!< mgroups this node corresponds to */
    bor_iset_t op; /*!< Connections to op nodes */
    int is_goal; /*!< True if one of the mgroups contain goal fact */
};
typedef struct pddl_mgroup_op_bipartite_graph_mgroup
    pddl_mgroup_op_bipartite_graph_mgroup_t;

struct pddl_mgroup_op_bipartite_graph {
    pddl_mgroup_op_bipartite_graph_op_t *op; /*!< Operator nodes */
    int op_size;
    pddl_mgroup_op_bipartite_graph_mgroup_t *mgroup; /*< MGroup nodes */
    int mgroup_size;
};
typedef struct pddl_mgroup_op_bipartite_graph pddl_mgroup_op_bipartite_graph_t;

/**
 * Initialize bipartite graph between mutex groups and operators.
 */
void pddlMGroupOpBipartiteGraphInit(pddl_mgroup_op_bipartite_graph_t *g,
                                    const pddl_strips_t *strips,
                                    const pddl_strips_cross_ref_t *cref);

/**
 * Free allocated memory.
 */
void pddlMGroupOpBipartiteGraphFree(pddl_mgroup_op_bipartite_graph_t *g);

/**
 * Minimize edges by merging operator nodes that are incident with the same
 * mgroup nodes; and the operator nodes incident with only one mgroup node
 * are removed.
 */
void pddlMGroupOpBipartiteGraphMinimize(pddl_mgroup_op_bipartite_graph_t *g);

/**
 * Merge two mgroup nodes; src node is merged in dst node.
 */
void pddlMGroupOpBipartiteGraphMerge(pddl_mgroup_op_bipartite_graph_t *g,
                                     int dst, int src);


void pddlMGroupOpBipartiteGraphPrint(const pddl_mgroup_op_bipartite_graph_t *g,
                                     FILE *fout);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_MGROUP_OP_BIPARTITE_GRAPH_H__ */
