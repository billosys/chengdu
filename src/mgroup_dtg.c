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

#include <boruvka/alloc.h>

#include "pddl/pddl.h"
#include "pddl/mgroup_dtg.h"
#include "err.h"

static void nodeInit(pddl_mgroup_dtg_node_t *node, int fact_id, int node_size)
{
    node->fact = fact_id;
    node->edge = BOR_ALLOC_ARR(pddl_mgroup_dtg_edge_t, node_size);
    node->edge_size = 0;
    for (int i = 0; i < node_size; ++i){
        node->edge[i].node = i;
        borISetInit(&node->edge[i].op);
    }
}

static void nodeFree(pddl_mgroup_dtg_node_t *node)
{
    for (int i = 0; i < node->edge_size; ++i)
        borISetFree(&node->edge[i].op);
    if (node->edge != NULL)
        BOR_FREE(node->edge);
}

static void finalizeNode(pddl_mgroup_dtg_node_t *node, int node_size)
{
    node->edge_size = 0;
    for (int i = 0; i < node_size; ++i){
        if (borISetSize(&node->edge[i].op) > 0)
            node->edge[node->edge_size++] = node->edge[i];
    }
    node->edge = BOR_REALLOC_ARR(node->edge, pddl_mgroup_dtg_edge_t,
                                 node->edge_size);
}

static void addEdge(pddl_mgroup_dtg_t *dtg, int from, int to, int op_id)
{
    pddl_mgroup_dtg_node_t *from_node = dtg->node + from;
    pddl_mgroup_dtg_edge_t *edge = from_node->edge + to;
    borISetAdd(&edge->op, op_id);
}

int pddlMGroupDTGInit(pddl_mgroup_dtg_t *dtg,
                      const pddl_mgroup_t *mg,
                      const pddl_strips_t *strips)
{
    int *fact_to_node;
    bor_iset_t pre, add;

    if (strips->has_cond_eff)
        ERR_RET2(-1, "Conditional effects are not supported for mutex"
                     " group DTGs.");

    dtg->node_size = mg->fact.size + 1;
    dtg->node = BOR_CALLOC_ARR(pddl_mgroup_dtg_node_t, dtg->node_size);

    fact_to_node = BOR_CALLOC_ARR(int, strips->fact.fact_size);
    for (int i = 0; i < mg->fact.size; ++i){
        nodeInit(dtg->node + i, mg->fact.s[i], dtg->node_size);
        fact_to_node[mg->fact.s[i]] = i;
    }
    nodeInit(dtg->node + dtg->node_size - 1, -1, dtg->node_size);

    borISetInit(&pre);
    borISetInit(&add);
    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        borISetIntersect2(&pre, &mg->fact, &op->pre);
        borISetIntersect(&pre, &op->del_eff);
        if (borISetSize(&pre) > 0){
            borISetIntersect2(&add, &mg->fact, &op->add_eff);
            if (borISetSize(&add) > 0){
                addEdge(dtg, fact_to_node[pre.s[0]], fact_to_node[add.s[0]], i);
            }else{
                addEdge(dtg, fact_to_node[pre.s[0]], dtg->node_size - 1, i);
            }
        }
    }

    borISetFree(&pre);
    borISetFree(&add);
    BOR_FREE(fact_to_node);

    for (int i = 0; i < dtg->node_size; ++i)
        finalizeNode(dtg->node + i, dtg->node_size);

    return 0;
}

void pddlMGroupDTGFree(pddl_mgroup_dtg_t *dtg)
{
    for (int i = 0; i < dtg->node_size; ++i)
        nodeFree(dtg->node + i);
    if (dtg->node != NULL)
        BOR_FREE(dtg->node);
}

void pddlMGroupDTGPrintAsDot(const pddl_mgroup_dtg_t *dtg,
                             const pddl_t *pddl,
                             FILE *fout)
{
    fprintf(fout, "digraph {\n");
    for (int i = 0; i < dtg->node_size; ++i){
        fprintf(fout, "N%d [", i);
        if (dtg->node[i].fact >= 0){
            const pddl_fact_t *f = pddl->strips->fact.fact[dtg->node[i].fact];
            fprintf(fout, "label=\"%s\"", pddlFactToStr(pddl, f));
            if (borISetIn(dtg->node[i].fact, &pddl->strips->init))
                fprintf(fout, ", color=\"#bb2222\"");
            if (borISetIn(dtg->node[i].fact, &pddl->strips->goal))
                fprintf(fout, ", color=\"#2222bb\"");
        }else{
            fprintf(fout, "label=\"none-of-those\"");
        }
        fprintf(fout, "];\n");
    }

    for (int i = 0; i < dtg->node_size; ++i){
        const pddl_mgroup_dtg_node_t *n = dtg->node + i;
        for (int j = 0; j < n->edge_size; ++j){
            fprintf(fout, "N%d -> N%d [", i, n->edge[j].node);
            fprintf(fout, "label=\"|%d|\"", n->edge[j].op.size);
            fprintf(fout, "];\n");
        }
    }
    fprintf(fout, "}\n");
}
