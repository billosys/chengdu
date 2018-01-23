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
#include "pddl/mgroup_op_bipartite_graph.h"

void pddlMGroupOpBipartiteGraphInit(pddl_mgroup_op_bipartite_graph_t *g,
                                    const pddl_strips_t *strips,
                                    const pddl_strips_cross_ref_t *cref)
{
    bzero(g, sizeof(*g));
    g->op_size = strips->op.op_size;
    g->op = BOR_CALLOC_ARR(pddl_mgroup_op_bipartite_graph_op_t, g->op_size);
    g->mgroup_size = strips->mgroup.mgroup_size;
    g->mgroup = BOR_CALLOC_ARR(pddl_mgroup_op_bipartite_graph_mgroup_t,
                               g->mgroup_size);

    for (int i = 0; i < strips->op.op_size; ++i)
        borISetAdd(&g->op[i].op, i);

    for (int i = 0; i < strips->mgroup.mgroup_size; ++i){
        borISetAdd(&g->mgroup[i].mgroup, i);
        borISetUnion(&g->mgroup[i].op, &cref->mgroup[i].op_del_add);
        g->mgroup[i].is_goal = strips->mgroup.mgroup[i].is_goal;

        int op;
        BOR_ISET_FOR_EACH(&cref->mgroup[i].op_del_add, op)
            borISetAdd(&g->op[op].mgroup, i);
    }
}

void pddlMGroupOpBipartiteGraphFree(pddl_mgroup_op_bipartite_graph_t *g)
{
    for (int i = 0; i < g->op_size; ++i){
        borISetFree(&g->op[i].mgroup);
        borISetFree(&g->op[i].op);
    }
    if (g->op != NULL)
        BOR_FREE(g->op);

    for (int i = 0; i < g->mgroup_size; ++i){
        borISetFree(&g->mgroup[i].mgroup);
        borISetFree(&g->mgroup[i].op);
    }
    if (g->mgroup != NULL)
        BOR_FREE(g->mgroup);
}

static void removeUnaryOpNodes(pddl_mgroup_op_bipartite_graph_t *g)
{
    for (int i = 0; i < g->op_size; ++i){
        pddl_mgroup_op_bipartite_graph_op_t *op = g->op + i;
        if (borISetSize(&op->mgroup) == 1){
            borISetRm(&g->mgroup[borISetGet(&op->mgroup, 0)].op, i);
            borISetEmpty(&op->mgroup);
            borISetEmpty(&op->op);
        }
    }
}

static int minimizeCmp(const void *a, const void *b, void *ud)
{
    const int id1 = *(int *)a;
    const int id2 = *(int *)b;
    pddl_mgroup_op_bipartite_graph_t *graph = ud;
    const pddl_mgroup_op_bipartite_graph_op_t *op1 = graph->op + id1;
    const pddl_mgroup_op_bipartite_graph_op_t *op2 = graph->op + id2;
    return borISetCmp(&op1->mgroup, &op2->mgroup);
}

void pddlMGroupOpBipartiteGraphMinimize(pddl_mgroup_op_bipartite_graph_t *g)
{
    int *ops, cur;

    // First remove ops with only one incident mgroup
    removeUnaryOpNodes(g);

    // Sort operators according to the incident mgroups
    ops = BOR_ALLOC_ARR(int, g->op_size);
    for (int i = 0; i < g->op_size; ++i)
        ops[i] = i;
    borSort(ops, g->op_size, sizeof(int), minimizeCmp, g);

    // skip empty ops
    cur = 0;
    for (;cur < g->op_size && borISetSize(&g->op[ops[cur]].mgroup) == 0; ++cur);

    // join ops that are incident with the same mgroups
    for (int prev = cur++; cur < g->op_size; ++cur){
        pddl_mgroup_op_bipartite_graph_op_t *op_prev = g->op + ops[prev];
        pddl_mgroup_op_bipartite_graph_op_t *op_cur = g->op + ops[cur];
        if (borISetEq(&op_prev->mgroup, &op_cur->mgroup)){
            int mgi;
            BOR_ISET_FOR_EACH(&op_prev->mgroup, mgi)
                borISetRm(&g->mgroup[mgi].op, ops[prev]);
            borISetUnion(&op_cur->op, &op_prev->op);
            borISetEmpty(&op_prev->mgroup);
            borISetEmpty(&op_prev->op);
        }
        prev = cur;
    }

    BOR_FREE(ops);
}

void pddlMGroupOpBipartiteGraphMerge(pddl_mgroup_op_bipartite_graph_t *g,
                                     int dst_id, int src_id)
{
    pddl_mgroup_op_bipartite_graph_mgroup_t *dst = g->mgroup + dst_id;
    pddl_mgroup_op_bipartite_graph_mgroup_t *src = g->mgroup + src_id;
    int opi;

    borISetUnion(&dst->mgroup, &src->mgroup);
    borISetUnion(&dst->op, &src->op);
    dst->is_goal |= src->is_goal;

    BOR_ISET_FOR_EACH(&src->op, opi){
        borISetRm(&g->op[opi].mgroup, src_id);
        borISetAdd(&g->op[opi].mgroup, dst_id);
    }

    borISetEmpty(&src->mgroup);
    borISetEmpty(&src->op);
    src->is_goal = 0;
}

void pddlMGroupOpBipartiteGraphPrint(const pddl_mgroup_op_bipartite_graph_t *g,
                                     FILE *fout)
{

    for (int i = 0; i < g->op_size; ++i){
        if (borISetSize(&g->op[i].mgroup) == 0)
            continue;
        fprintf(fout, "Op %d:", i);
        int mg;
        BOR_ISET_FOR_EACH(&g->op[i].mgroup, mg)
            fprintf(fout, " %d", mg);
        fprintf(fout, " | ");
        int op;
        BOR_ISET_FOR_EACH(&g->op[i].op, op)
            fprintf(fout, " %d", op);
        fprintf(fout, "\n");

    }
    for (int i = 0; i < g->mgroup_size; ++i){
        fprintf(fout, "MG %d:", i);
        if (g->mgroup[i].is_goal)
            fprintf(fout, "g:");
        fprintf(fout, " [");
        int mg;
        BOR_ISET_FOR_EACH(&g->mgroup[i].mgroup, mg)
            fprintf(fout, " %d", mg);
        fprintf(fout, " ]");
        int op;
        BOR_ISET_FOR_EACH(&g->mgroup[i].op, op)
            fprintf(fout, " %d", op);
        fprintf(fout, "\n");

    }
}
