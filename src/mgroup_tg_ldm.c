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


#include <limits.h>
#include <boruvka/alloc.h>
#include "pddl/mgroup_tg_ldm.h"
#include "err.h"

struct node {
    int goal;
    int init;
    bor_iset_t in_edge;
};
typedef struct node node_t;

struct edge {
    int cost;
    int cost_init;
};
typedef struct edge edge_t;

struct graph {
    const pddl_g_t *tg;
    node_t *node;
    edge_t *edge;
};
typedef struct graph graph_t;

static void graphInit(graph_t *g, const pddl_g_t *tg,
                      const pddl_strips_t *strips)
{
    g->tg = tg;
    g->node = BOR_CALLOC_ARR(node_t, tg->node_size);
    g->edge = BOR_CALLOC_ARR(edge_t, tg->edge_size);

    for (int i = 0; i < tg->node_size; ++i){
        const pddl_g_node_t *n = tg->node + i;
        int edgei;
        BOR_ISET_FOR_EACH(&n->edge, edgei){
            const pddl_g_edge_t *e = tg->edge + edgei;
            borISetAdd(&g->node[e->to].in_edge, edgei);

            int opi, cost = INT_MAX;
            BOR_ISET_FOR_EACH(&e->label, opi)
                cost = BOR_MIN(cost, strips->op.op[opi]->cost);
            g->edge[edgei].cost_init = cost;
        }
    }
}

static void graphFree(graph_t *g)
{
    for (int i = 0; i < g->tg->node_size; ++i){
        borISetFree(&g->node[i].in_edge);
    }
    if (g->node != NULL)
        BOR_FREE(g->node);
    if (g->edge != NULL)
        BOR_FREE(g->edge);
}

static void graphResetCost(graph_t *g)
{
    for (int i = 0; i < g->tg->edge_size; ++i)
        g->edge[i].cost = g->edge[i].cost_init;
}

static void graphMarkGoalZoneNode(graph_t *g, int node)
{
    int edgei;

    if (g->node[node].goal)
        return;
    g->node[node].goal = 1;
    BOR_ISET_FOR_EACH(&g->node[node].in_edge, edgei){
        if (g->edge[edgei].cost == 0)
            graphMarkGoalZoneNode(g, g->tg->edge[edgei].from);
    }

}

static void graphMarkGoalZone(graph_t *g, const bor_iset_t *goal)
{
    int node;
    for (int i = 0; i < g->tg->node_size; ++i)
        g->node[i].goal = 0;
    BOR_ISET_FOR_EACH(goal, node)
        graphMarkGoalZoneNode(g, node);
}

static void graphFindCutNode(graph_t *g, int node, bor_iset_t *cut)
{
    int edgei;

    if (g->node[node].init)
        return;
    g->node[node].init = 1;

    BOR_ISET_FOR_EACH(&g->tg->node[node].edge, edgei){
        const pddl_g_edge_t *edge = g->tg->edge + edgei;
        if (g->node[edge->to].goal){
            borISetAdd(cut, edgei);
        }else{
            graphFindCutNode(g, edge->to, cut);
        }
    }
}

static void graphFindCut(graph_t *g, int init_node, bor_iset_t *cut)
{
    if (g->node[init_node].goal)
        return;

    for (int i = 0; i < g->tg->node_size; ++i)
        g->node[i].init = 0;
    graphFindCutNode(g, init_node, cut);
}

static void graphApplyCut(graph_t *g, const bor_iset_t *cut)
{
    int cut_cost, edge;

    cut_cost = INT_MAX;
    BOR_ISET_FOR_EACH(cut, edge)
        cut_cost = BOR_MIN(cut_cost, g->edge[edge].cost);
    BOR_ISET_FOR_EACH(cut, edge)
        g->edge[edge].cost -= cut_cost;
}

static void graphLdmSeq(graph_t *g,
                        int init_node,
                        const bor_iset_t *goal_nodes,
                        pddl_landmarks_t *ldms,
                        bor_iarr_t *ldm_seq)
{
    BOR_ISET(cut);
    BOR_ISET(ldm);
    int edgei;

    graphResetCost(g);
    do {
        graphMarkGoalZone(g, goal_nodes);
        borISetEmpty(&cut);
        graphFindCut(g, init_node, &cut);
        if (borISetSize(&cut) > 0){
            borISetEmpty(&ldm);
            BOR_ISET_FOR_EACH(&cut, edgei)
                borISetUnion(&ldm, &g->tg->edge[edgei].label);
            borIArrPrepend(ldm_seq, pddlLandmarksAdd(ldms, &ldm)->id);

            fprintf(stderr, "Cut:");
            BOR_ISET_FOR_EACH(&cut, edgei)
                fprintf(stderr, " %d[%d]", edgei, g->edge[edgei].cost);
            fprintf(stderr, "\n");

            graphApplyCut(g, &cut);
        }
    } while (borISetSize(&cut) > 0);

    borISetFree(&ldm);
    borISetFree(&cut);
}


static pddl_mgroup_tg_ldm_tg_t *tgNew(void)
{
    pddl_mgroup_tg_ldm_tg_t *tg;
    tg = BOR_ALLOC(pddl_mgroup_tg_ldm_tg_t);
    bzero(tg, sizeof(*tg));
    return tg;
}

static void tgDel(pddl_mgroup_tg_ldm_tg_t *tg)
{
    borISetFree(&tg->mgroup);
    borISetFree(&tg->tg_op);
    borISetFree(&tg->tg_fact);
    borISetFree(&tg->init_node);
    borISetFree(&tg->goal_node);
    borIArrFree(&tg->core_ldm_seq);
    BOR_FREE(tg);
}

static void tgFinalize(pddl_mgroup_tg_ldm_tg_t *tg,
                       const pddl_strips_t *strips)
{
    BOR_ISET(init);
    BOR_ISET(goal);

    pddlGUnionEdgeLabels(&tg->tg, &tg->tg_op);
    pddlGUnionNodeLabels(&tg->tg, &tg->tg_fact);

    borISetIntersect2(&init, &tg->tg_fact, &strips->init);
    borISetIntersect2(&goal, &tg->tg_fact, &strips->goal);
    for (int nid = 0; nid < tg->tg.node_size; ++nid){
        const pddl_g_node_t *node = tg->tg.node + nid;
        if (borISetIsSubset(&init, &node->label))
            borISetAdd(&tg->init_node, nid);
        if (borISetIsSubset(&goal, &node->label))
            borISetAdd(&tg->goal_node, nid);
    }

    borISetFree(&init);
    borISetFree(&goal);
}

static pddl_mgroup_tg_ldm_tg_t *nextTG(pddl_mgroup_tg_ldm_t *m)
{
    pddl_mgroup_tg_ldm_tg_t *tg;

    if (m->tg_size == m->tg_alloc){
        m->tg_alloc *= 2;
        m->tg = BOR_REALLOC_ARR(m->tg, pddl_mgroup_tg_ldm_tg_t *, m->tg_alloc);
    }

    tg = tgNew();
    m->tg[m->tg_size++] = tg;
    return tg;
}

static void nextLdm(const pddl_g_t *tg,
                    const bor_iset_t *init,
                    const bor_iset_t *goal,
                    bor_iset_t *next_goal,
                    bor_iset_t *ldm,
                    int *visited)
{
    int edge;

    borISetEmpty(next_goal);
    borISetEmpty(ldm);
    bzero(visited, sizeof(int) * tg->node_size);

    pddlGDFS(tg, init, goal, visited);
    for (int i = 0; i < tg->node_size; ++i){
        if (!visited[i])
            continue;
        if (borISetIn(i, goal))
            continue;
        BOR_ISET_FOR_EACH(&tg->node[i].edge, edge){
            const pddl_g_edge_t *e = tg->edge + edge;
            if (borISetIn(e->to, goal)){
                borISetUnion(ldm, &e->label);
                borISetAdd(next_goal, i);
            }
        }
    }
}

static void ldmSeq(const pddl_g_t *tg,
                   const bor_iset_t *init_node,
                   const bor_iset_t *goal_node,
                   bor_iarr_t *lseq,
                   pddl_landmarks_t *ldms)
{
    BOR_ISET(next_goal);
    BOR_ISET(init);
    BOR_ISET(goal);
    BOR_ISET(ldm);
    int *visited;
    int prev_ldm_id = -1, ldm_id;

    if (borISetIsSubset(goal_node, init_node))
        return;

    borISetMinus2(&init, init_node, goal_node);
    borISetUnion(&goal, goal_node);
    visited = BOR_ALLOC_ARR(int, tg->node_size);
    while (borISetSize(&init) > 0 && borISetSize(&goal) > 0){
        // Find next landmark
        nextLdm(tg, init_node, &goal, &next_goal, &ldm, visited);
        if (borISetSize(&ldm) == 0)
            break;

        // Add a new landmark, terminate if we got the same landmark as in
        // the previous step.
        ldm_id = pddlLandmarksAdd(ldms, &ldm)->id;
        if (prev_ldm_id == ldm_id)
            break;
        borIArrPrepend(lseq, ldm_id);

        // Update goal with the nodes reached with the new landmark minus
        // those that are already in init.
        borISetUnion(&goal, &next_goal);
        borISetMinus(&goal, &init);

        fprintf(stderr, "GoalStep:");
        int f;
        BOR_ISET_FOR_EACH(&goal, f)
            fprintf(stderr, " %d", f);
        fprintf(stderr, "\n");

        prev_ldm_id = ldm_id;
    }

    borISetFree(&next_goal);
    borISetFree(&init);
    borISetFree(&goal);
    borISetFree(&ldm);
    BOR_FREE(visited);
}

static void createMGroupTGs(pddl_mgroup_tg_ldm_t *m,
                            const pddl_strips_t *strips)
{
    m->mgroup_tg = BOR_ALLOC_ARR(pddl_g_t, strips->mgroup.size);
    for (int i = 0; i < strips->mgroup.size; ++i)
        pddlMGroupTGInit(&m->mgroup_tg[i], strips->mgroup.g + i, strips);
}

static void createTGs(pddl_mgroup_tg_ldm_t *m,
                      const pddl_strips_t *strips)
{
    pddl_mgroup_tg_ldm_tg_t *tg;

    for (int i = 0; i < strips->mgroup.size; ++i){
        tg = nextTG(m);
        pddlMGroupTGInit(&tg->tg, strips->mgroup.g + i, strips);
        pddlGPrintDebug(&tg->tg, stderr);
        borISetAdd(&tg->mgroup, i);
        tgFinalize(tg, strips);
        graph_t ldmg;
        graphInit(&ldmg, &tg->tg, strips);
        graphLdmSeq(&ldmg, borISetGet(&tg->init_node, 0),
                &tg->goal_node, &m->ldm, &tg->core_ldm_seq);
        graphFree(&ldmg);
            int f;
            fprintf(stderr, "InitF:");
            BOR_ISET_FOR_EACH(&strips->init, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");
            fprintf(stderr, "GoalF:");
            BOR_ISET_FOR_EACH(&strips->goal, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");
            fprintf(stderr, "I:");
            BOR_ISET_FOR_EACH(&tg->init_node, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");
            fprintf(stderr, "G:");
            BOR_ISET_FOR_EACH(&tg->goal_node, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");

            BOR_IARR_FOR_EACH(&tg->core_ldm_seq, f){
                int o;
                fprintf(stderr, "LDM %d:", f);
                BOR_ISET_FOR_EACH(&m->ldm.ldm[f]->op, o)
                    fprintf(stderr, " %d[%d]", o, strips->op.op[o]->cost);
                fprintf(stderr, "\n");
            }
    }

    for (int i = 0; i < strips->mgroup.size; ++i){
        for (int j = i + 1; j < strips->mgroup.size; ++j){
            if (!strips->mgroup.g[i].is_goal
                    && !strips->mgroup.g[i].is_goal)
                continue;
            fprintf(stderr, "=====\n");
            tg = nextTG(m);
            pddlMGroupTGSyncProduct(&tg->tg, &m->mgroup_tg[i],
                                    &m->mgroup_tg[j], strips);
            pddlGPrintDebug(&tg->tg, stderr);
            borISetAdd(&tg->mgroup, i);
            borISetAdd(&tg->mgroup, j);
            tgFinalize(tg, strips);
            //ldmSeq(&tg->tg, &tg->init_node, &tg->goal_node, 
            //       &tg->core_ldm_seq, &m->ldm);

            graph_t ldmg;
            graphInit(&ldmg, &tg->tg, strips);
            graphLdmSeq(&ldmg, borISetGet(&tg->init_node, 0),
                        &tg->goal_node, &m->ldm, &tg->core_ldm_seq);
            graphFree(&ldmg);

            int f;
            fprintf(stderr, "InitF:");
            BOR_ISET_FOR_EACH(&strips->init, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");
            fprintf(stderr, "GoalF:");
            BOR_ISET_FOR_EACH(&strips->goal, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");
            fprintf(stderr, "I:");
            BOR_ISET_FOR_EACH(&tg->init_node, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");
            fprintf(stderr, "G:");
            BOR_ISET_FOR_EACH(&tg->goal_node, f)
                fprintf(stderr, " %d", f);
            fprintf(stderr, "\n");

            BOR_IARR_FOR_EACH(&tg->core_ldm_seq, f){
                int o;
                fprintf(stderr, "LDM %d:", f);
                BOR_ISET_FOR_EACH(&m->ldm.ldm[f]->op, o)
                    fprintf(stderr, " %d[%d]", o, strips->op.op[o]->cost);
                fprintf(stderr, "\n");
            }
        }
    }
}

void pddlMGroupTGLdmInit(pddl_mgroup_tg_ldm_t *m, const pddl_strips_t *strips)
{
    bzero(m, sizeof(*m));
    m->strips = strips;
    pddlLandmarksInit(&m->ldm);
    m->tg_alloc = 2;
    m->tg = BOR_ALLOC_ARR(pddl_mgroup_tg_ldm_tg_t *, m->tg_alloc);

    createMGroupTGs(m, strips);
    createTGs(m, strips);
}

void pddlMGroupTGLdmFree(pddl_mgroup_tg_ldm_t *m)
{
    pddlLandmarksFree(&m->ldm);
    for (int i = 0; i < m->tg_size; ++i)
        tgDel(m->tg[i]);
    if (m->tg != NULL)
        BOR_FREE(m->tg);
}
