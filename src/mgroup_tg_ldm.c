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
#include <boruvka/lp.h>
#include <boruvka/alloc.h>
#include "pddl/mgroup_tg_ldm.h"
#include "err.h"
#include "assert.h"

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
        BOR_ISET_FOR_EACH(&n->out, edgei){
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

    BOR_ISET_FOR_EACH(&g->tg->node[node].out, edgei){
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

static int graphCutCost(graph_t *g, const bor_iset_t *cut)
{
    int cut_cost, edge;

    cut_cost = INT_MAX;
    BOR_ISET_FOR_EACH(cut, edge)
        cut_cost = BOR_MIN(cut_cost, g->edge[edge].cost);
    return cut_cost;
}

static void graphApplyCut(graph_t *g, const bor_iset_t *cut, int cut_cost)
{
    int edge;
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
    int edgei, cost;
    pddl_landmark_t *lldm;

    graphResetCost(g);
    do {
        graphMarkGoalZone(g, goal_nodes);
        borISetEmpty(&cut);
        graphFindCut(g, init_node, &cut);
        if (borISetSize(&cut) > 0){
            borISetEmpty(&ldm);
            BOR_ISET_FOR_EACH(&cut, edgei)
                borISetUnion(&ldm, &g->tg->edge[edgei].label);
            cost = graphCutCost(g, &cut);
            lldm = pddlLandmarksAdd(ldms, &ldm);
            lldm->cost = BOR_MAX(lldm->cost, cost);
            borIArrPrepend(ldm_seq, lldm->id);

            fprintf(stderr, "Cut:");
            BOR_ISET_FOR_EACH(&cut, edgei)
                fprintf(stderr, " %d[%d]", edgei, g->edge[edgei].cost);
            fprintf(stderr, "\n");

            graphApplyCut(g, &cut, cost);
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
        BOR_ISET_FOR_EACH(&tg->node[i].out, edge){
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

static void setCover(pddl_mgroup_tg_ldm_t *m,
                     const pddl_strips_t *strips)
{
    BOR_ISET(U);
    bor_lp_t *lp;
    unsigned lp_flags;
    int rows = 0;
    double rhs;
    char sense;

    for (int i = 0; i < m->ldm.ldm_size; ++i){
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        borISetUnion(&U, &ldm->op);
    }

    lp_flags  = BOR_LP_DEFAULT;
    lp_flags |= BOR_LP_NUM_THREADS(1); // TODO: Parametrize
    lp_flags |= BOR_LP_MIN;
    lp = borLPNew(0, m->ldm.ldm_size, lp_flags);

    for (int i = 0; i < m->ldm.ldm_size; ++i){
        borLPSetObj(lp, i, 1.);
        borLPSetVarBinary(lp, i);
    }

    int fact;
    BOR_ISET_FOR_EACH(&U, fact){
        rhs = 1;
        sense = 'G';
        borLPAddRows(lp, 1, &rhs, &sense);
        for (int i = 0; i < m->ldm.ldm_size; ++i){
            const pddl_landmark_t *ldm = m->ldm.ldm[i];
            if (borISetIn(fact, &ldm->op))
                borLPSetCoef(lp, rows, i, 1.);

        }
        ++rows;
    }
    
    double val;
    double *obj = BOR_ALLOC_ARR(double, m->ldm.ldm_size);
    ASSERT(borLPSolve(lp, &val, obj) == 0);
    for (int i = 0; i < m->ldm.ldm_size; ++i){
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        if (obj[i] > 0.5){
            fprintf(stderr, "SC %d %d\n", i, ldm->cost);
        }
    }
    fprintf(stderr, "SS: %f\n", val);

    BOR_FREE(obj);
    borLPDel(lp);
    borISetFree(&U);
}

static void hittingSet(pddl_mgroup_tg_ldm_t *m, const pddl_strips_t *strips)
{
    BOR_ISET(ops);
    int *U = BOR_CALLOC_ARR(int, strips->op.op_size);
    bor_lp_t *lp;
    unsigned lp_flags;
    int rows = 0;
    double rhs;
    char sense;
    int op;

    for (int i = 0; i < m->ldm.ldm_size; ++i){
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        borISetUnion(&ops, &ldm->op);
    }
    int idx = 0;
    BOR_ISET_FOR_EACH(&ops, op)
        U[op] = idx++;

    lp_flags  = BOR_LP_DEFAULT;
    lp_flags |= BOR_LP_NUM_THREADS(1); // TODO: Parametrize
    lp_flags |= BOR_LP_MIN;
    lp = borLPNew(0, idx, lp_flags);

    BOR_ISET_FOR_EACH(&ops, op){
        borLPSetObj(lp, U[op], strips->op.op[op]->cost);
        borLPSetVarBinary(lp, U[op]);
    }

    for (int i = 0; i < m->ldm.ldm_size; ++i){
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        rhs = 1.;
        sense = 'G';
        borLPAddRows(lp, 1, &rhs, &sense);
        BOR_ISET_FOR_EACH(&ldm->op, op){
            borLPSetCoef(lp, rows, U[op], 1.);
        }

        ++rows;
    }
    
    double val;
    double *obj = BOR_ALLOC_ARR(double, borISetSize(&ops));
    ASSERT(borLPSolve(lp, &val, obj) == 0);
    int cost = 0;
    for (int i = 0; i < borISetSize(&ops); ++i){
        if (obj[i] > .5){
            cost += strips->op.op[borISetGet(&ops, i)]->cost;
            fprintf(stderr, "%d:%d: %s\n",
                    borISetGet(&ops, i),
                    strips->op.op[borISetGet(&ops, i)]->cost,
                    strips->op.op[borISetGet(&ops, i)]->name);
        }
    }
    /*
    for (int i = 0; i < m->ldm.ldm_size; ++i){
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        if (obj[i] > 0.5){
            fprintf(stderr, "SC %d %d\n", i, ldm->cost);
        }
    }
    */
    fprintf(stderr, "SS: %f, %d\n", val, cost);

    BOR_FREE(obj);
    borLPDel(lp);
    BOR_FREE(U);
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
                    && !strips->mgroup.g[j].is_goal)
                continue;
            //if (borISetIsDisjunct(&strips->mgroup.g[i].fact,
            //                      &strips->mgroup.g[j].fact))
            //    continue;
            fprintf(stderr, "=====\n");
            tg = nextTG(m);
            pddlMGroupTGSyncProduct(&tg->tg, &m->mgroup_tg[i],
                                    &m->mgroup_tg[j], strips);
            fprintf(stderr, "SIZE: %d\n", tg->tg.node_size);
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
            /*
            BOR_ISET(g);
            int gn;
            BOR_ISET_FOR_EACH(&tg->goal_node, gn){
                borISetEmpty(&g);
                borISetAdd(&g, gn);
                graphLdmSeq(&ldmg, borISetGet(&tg->init_node, 0),
                            &g, &m->ldm, &tg->core_ldm_seq);
            }
            borISetFree(&g);
            */
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

    int *use_ldm = BOR_CALLOC_ARR(int, m->ldm.ldm_size);
    for (int i = 0; i < m->ldm.ldm_size; ++i){
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        for (int j = i + 1; j < m->ldm.ldm_size; ++j){
            const pddl_landmark_t *ldm2 = m->ldm.ldm[j];
            if (borISetIsSubset(&ldm->op, &ldm2->op)){
                fprintf(stderr, "RM %d\n", ldm2->id);
                use_ldm[ldm2->id] = 1;
            }else if (borISetIsSubset(&ldm2->op, &ldm->op)){
                fprintf(stderr, "RM %d\n", ldm->id);
                use_ldm[ldm->id] = 1;
            }
        }
    }
    for (int i = 0; i < m->ldm.ldm_size; ++i){
        if (use_ldm[i] != 0)
            continue;
        fprintf(stderr, "Keep %d\n", i);
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        fprintf(stderr, "    %d ::", ldm->cost);
        int op;
        BOR_ISET_FOR_EACH(&ldm->op, op)
            fprintf(stderr, " %d", op);
        fprintf(stderr, "\n");
    }
    BOR_FREE(use_ldm);


    BOR_ISET(all_ops);
    for (int i = 0; i < strips->op.op_size; ++i)
        borISetAdd(&all_ops, i);
    BOR_ISET(ops);
    int cost = 0;
    for (int i = 0; i < m->ldm.ldm_size; ++i){
        const pddl_landmark_t *ldm = m->ldm.ldm[i];
        borISetUnion(&ops, &ldm->op);
        cost += ldm->cost;
    }
    int op;
    fprintf(stderr, "All cost: %d\n", cost);
    BOR_ISET_FOR_EACH(&ops, op)
        fprintf(stderr, " %d", op);
    fprintf(stderr, "\n");
    BOR_ISET(remain_op);
    borISetMinus2(&remain_op, &all_ops, &ops);
    fprintf(stderr, "remain:");
    BOR_ISET_FOR_EACH(&remain_op, op)
        fprintf(stderr, " %d", op);
    fprintf(stderr, "\n");
    borISetFree(&ops);

    setCover(m, strips);
    hittingSet(m, strips);

    for (int i = 0; i < strips->mgroup.size; ++i){
        if (!strips->mgroup.g[i].is_goal)
            continue;
        for (int j = i + 1; j < strips->mgroup.size; ++j){
            if (!strips->mgroup.g[j].is_goal)
                continue;
            pddl_g_t tg1;
            pddlMGroupTGSyncProduct(&tg1, &m->mgroup_tg[i],
                                    &m->mgroup_tg[j], strips);
            INFO("X %d %d", i, j);
            for (int k = j + 1; k < strips->mgroup.size; ++k){
                if (!strips->mgroup.g[k].is_goal)
                    continue;
                pddl_g_t tg2;
                pddlMGroupTGSyncProduct(&tg2, &tg1, &m->mgroup_tg[k], strips);
                INFO("X %d %d %d -- %d", i, j, k, tg2.node_size);
                for (int l = k + 1; l < strips->mgroup.size; ++l){
                    if (!strips->mgroup.g[l].is_goal)
                        continue;
                    pddl_g_t tg3;
                    pddlMGroupTGSyncProduct(&tg3, &tg2, &m->mgroup_tg[l], strips);
                    INFO("X %d %d %d %d -- %d, %d", i, j, k, l,
                            tg3.node_size, tg3.edge_size);
                    pddlGFree(&tg3);
                }
                pddlGFree(&tg2);
            }
            pddlGFree(&tg1);
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
