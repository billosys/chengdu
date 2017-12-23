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

#include <boruvka/sort.h>
#include "pddl/strips.h"
#include "pddl/mgroup_ldm_pot.h"
#include "pddl/util.h"
#include "pddl/err.h"
#include "err.h"
#include "assert.h"

struct fact {
    bor_iset_t op_del; /*!< Operators that delete this fact */
    bor_iset_t op_add; /*!< Operators that add this fact */
};
typedef struct fact fact_t;

struct mgroup {
    int id;
    bor_iset_t fact;
    int none_of_those;
    bor_iset_t op;
    bor_iset_t op_add;
    bor_iset_t op_del;
    int is_goal;
};
typedef struct mgroup mgroup_t;

struct pot {
    pddl_strips_t *strips;
    fact_t *fact;
    mgroup_t *mgroup;
    int mgroup_size;
    int mgroup_alloc;
    bor_iset_t *sync_prod;
    int sync_prod_size;
};
typedef struct pot pot_t;

struct tg_edge {
    int to;
    int cost;
} bor_packed;
typedef struct tg_edge tg_edge_t;

struct tg_node {
    int *fact;
    int fact_size;
    tg_edge_t *next;
    int next_size;
    tg_edge_t *prev;
    int prev_size;
} bor_packed;
typedef struct tg_node tg_node_t;

struct tg_graph {
    tg_node_t *node;
    int node_size;
};
typedef struct tg_graph tg_graph_t;

/** Plan and prepare synchronized products of mgroups */
static void sprodPrepare(pot_t *pot);
static unsigned long tgNumNodes(const pot_t *pot, const bor_iset_t *mg);
static int tgCanFitInMem(unsigned long num_nodes,
                         unsigned long num_mgroups, unsigned long mem);



static int tgCanFitInMem(unsigned long num_nodes,
                         unsigned long num_mgroups, unsigned long mem)
{
    unsigned long req_mem;

    req_mem  = num_nodes * sizeof(tg_node_t);
    req_mem += num_nodes * num_nodes * 2 * sizeof(tg_edge_t);
    req_mem += num_nodes * num_mgroups * sizeof(int);
    return req_mem < mem;
}

static unsigned long tgNumNodesRec(const pot_t *pot,
                                   const bor_iset_t *mg, int mg_idx,
                                   const bor_iset_t *facts)
{
    BOR_ISET(fs);
    int mg_id, f;
    unsigned long num_nodes = 0UL;

    mg_id = borISetGet(mg, mg_idx);
    BOR_ISET_FOR_EACH(&pot->mgroup[mg_id].fact, f){
        borISetEmpty(&fs);
        borISetUnion(&fs, facts);
        borISetAdd(&fs, f);
        if (mg_idx == borISetSize(mg) - 1){
            if (!pddlMutexesIsMutex(&pot->strips->mutex, &fs))
                num_nodes += 1UL;
        }else{
            num_nodes += tgNumNodesRec(pot, mg, mg_idx + 1, &fs);
        }
    }

    borISetFree(&fs);
    return num_nodes;
}

static unsigned long tgNumNodes(const pot_t *pot, const bor_iset_t *mg)
{
    BOR_ISET(facts);
    unsigned long num_nodes;
    int mgi;

    num_nodes = 1;
    BOR_ISET_FOR_EACH(mg, mgi)
        num_nodes *= borISetSize(&pot->mgroup[mgi].fact);
    return num_nodes;

    num_nodes = tgNumNodesRec(pot, mg, 0, &facts);
    borISetFree(&facts);

    return num_nodes;
}

static void mgroupMakeExactlyOne(mgroup_t *mgroup, pddl_strips_t *strips)
{
    pddl_fact_t fact;
    char name[1024], *s;
    int fid;

    s = name;
    s += sprintf(s, "NOT");
    BOR_ISET_FOR_EACH(&mgroup->fact, fid)
        s += sprintf(s, ":%s", strips->fact.fact[fid]->name);
    pddlFactInit(&fact);
    fact.name = BOR_STRDUP(name);
    mgroup->none_of_those = pddlFactsAdd(&strips->fact, &fact);
    pddlFactFree(&fact);

    if (borISetIsDisjunct(&mgroup->fact, &strips->init))
        borISetAdd(&strips->init, mgroup->none_of_those);
    borISetAdd(&mgroup->fact, mgroup->none_of_those);
}

static void mgroupAddSingle(pot_t *pot, int fact_id)
{
    mgroup_t *mg;
    BOR_ISET(new_goal);

    if (pot->mgroup_size >= pot->mgroup_alloc){
        pot->mgroup_alloc *= 2;
        pot->mgroup = BOR_REALLOC_ARR(pot->mgroup, mgroup_t, pot->mgroup_alloc);
    }

    mg = pot->mgroup + pot->mgroup_size;
    bzero(mg, sizeof(*mg));
    mg->id = pot->mgroup_size;
    borISetAdd(&mg->fact, fact_id);
    mgroupMakeExactlyOne(mg, pot->strips);
    if (borISetIn(fact_id, &pot->strips->goal)){
        mg->is_goal = 1;
    }else{
        borISetAdd(&new_goal, fact_id);
        borISetUnion(&new_goal, &pot->strips->goal);
        if (pddlMutexesIsMutex(&pot->strips->mutex, &new_goal)){
            INFO("Adding %s to goal",
                    pot->strips->fact.fact[borISetGet(&mg->fact, 1)]->name);
            borISetAdd(&pot->strips->goal, borISetGet(&mg->fact, 1));
            mg->is_goal = 1;
        }
    }

    ++pot->mgroup_size;
    borISetFree(&new_goal);
}

static void mgroupComplete(pot_t *pot)
{
    BOR_ISET(facts);
    int fact;

    for (int i = 0; i < pot->strips->fact.fact_size; ++i)
        borISetAdd(&facts, i);
    for (int i = 0; i < pot->mgroup_size; ++i)
        borISetMinus(&facts, &pot->mgroup[i].fact);

    BOR_ISET_FOR_EACH(&facts, fact)
        mgroupAddSingle(pot, fact);

    borISetFree(&facts);
}

static void potInit(pot_t *pot, const pddl_strips_t *strips)
{
    int fact_id, op_id;

    bzero(pot, sizeof(*pot));
    pot->strips = pddlStripsClone(strips);

    pot->mgroup_alloc = pot->mgroup_size = strips->mgroup.mgroup_size;
    pot->mgroup_alloc = BOR_MAX(pot->mgroup_alloc, 1);
    if (pot->mgroup_alloc > 0)
        pot->mgroup = BOR_CALLOC_ARR(mgroup_t, pot->mgroup_alloc);
    for (int mi = 0; mi < strips->mgroup.mgroup_size; ++mi){
        const pddl_mgroup_t *mg = strips->mgroup.mgroup + mi;
        pot->mgroup[mi].id = mi;
        pot->mgroup[mi].is_goal = mg->is_goal;
        borISetUnion(&pot->mgroup[mi].fact, &mg->fact);
        if (!mg->is_exactly_1)
            mgroupMakeExactlyOne(pot->mgroup + mi, pot->strips);
    }
    mgroupComplete(pot);

    pot->fact = BOR_CALLOC_ARR(fact_t, pot->strips->fact.fact_size);
    for (int opi = 0; opi < pot->strips->op.op_size; ++opi){
        const pddl_strips_op_t *op = pot->strips->op.op[opi];
        BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
            borISetAdd(&pot->fact[fact_id].op_del, opi);
        BOR_ISET_FOR_EACH(&op->add_eff, fact_id)
            borISetAdd(&pot->fact[fact_id].op_add, opi);
    }

    for (int mi = 0; mi < pot->mgroup_size; ++mi){
        BOR_ISET(ops);
        mgroup_t *mg = pot->mgroup + mi;

        BOR_ISET_FOR_EACH(&mg->fact, fact_id){
            borISetUnion(&mg->op_del, &pot->fact[fact_id].op_del);
            borISetUnion(&mg->op_add, &pot->fact[fact_id].op_add);
        }

        borISetMinus2(&ops, &mg->op_del, &mg->op_add);
        BOR_ISET_FOR_EACH(&ops, op_id)
            borISetAdd(&pot->fact[mg->none_of_those].op_add, op_id);

        borISetMinus2(&ops, &mg->op_add, &mg->op_del);
        BOR_ISET_FOR_EACH(&ops, op_id)
            borISetAdd(&pot->fact[mg->none_of_those].op_del, op_id);

        borISetUnion2(&mg->op, &mg->op_add, &mg->op_del);
        borISetFree(&ops);
    }
    INFO2("POT");

    printf("OPS:\n");
    printf("        ");
    for (int i = 0; i < pot->mgroup_size; ++i)
        printf(" % 3d", i);
    printf("\n");
    for (int i = 0; i < pot->mgroup_size; ++i){
        printf("% 3d:% 3d:", borISetSize(&pot->mgroup[i].fact), i);
        for (int j = 0; j < pot->mgroup_size; ++j){
            if (i != j){
            printf(" % 3d",
                    borISetIntersectionSize(&pot->mgroup[i].op,
                                            &pot->mgroup[j].op));
            }else{
                printf("    ");
            }
        }
        printf("\n");
    }

    printf("FACTS:\n");
    printf("        ");
    for (int i = 0; i < pot->mgroup_size; ++i)
        printf(" % 3d", i);
    printf("\n");
    for (int i = 0; i < pot->mgroup_size; ++i){
        printf("% 3d:% 3d:", borISetSize(&pot->mgroup[i].fact), i);
        for (int j = 0; j < pot->mgroup_size; ++j){
            if (i != j){
            printf(" % 3d",
                    borISetIntersectionSize(&pot->mgroup[i].fact,
                                            &pot->mgroup[j].fact));
            }else{
                printf("    ");
            }
        }
        printf("\n");
    }

    int i;
    unsigned long num_nodes = 1;
    BOR_ISET(tgmg);
    BOR_ISET(tgfacts);
    for (i = 0; i < pot->mgroup_size; ++i){
        /*
        if (borISetSize(&tgmg) == 0){
            num_nodes = borISetSize(&pot->mgroup[i].fact);
        }else{
            num_nodes *= borISetSize(&pot->mgroup[i].fact)
                            - borISetIntersectionSize(&tgfacts,
                                    &pot->mgroup[i].fact);
        }
        //num_nodes *= borISetSize(&pot->mgroup[i].fact);
        */
        borISetAdd(&tgmg, i);
        num_nodes = tgNumNodes(pot, &tgmg);
        printf("N: %lu\n", num_nodes);

        if (tgCanFitInMem(num_nodes, i + 1, 6UL * 1024UL * 1024UL * 1024UL)){
            //borISetAdd(&tgmg, i);
            borISetUnion(&tgfacts, &pot->mgroup[i].fact);
        }else{
            printf("!nodes: %lu\n", num_nodes);
            borISetRm(&tgmg, i);
            num_nodes = tgNumNodes(pot, &tgmg);
            printf("nodes: %lu, mgroups: %d\n", num_nodes, borISetSize(&tgmg));
            break;
        }
    }
    borISetFree(&tgmg);

    sprodPrepare(pot);
    for (int i = 0; i < pot->sync_prod_size; ++i){
        int mgi;
        printf("Sync product:");
        BOR_ISET_FOR_EACH(pot->sync_prod + i, mgi){
            printf(" %d", mgi);
            if (pot->mgroup[mgi].is_goal)
                printf("G");
        }
        printf("\n");
    }
}

static void potFree(pot_t *pot)
{
    for (int i = 0; i < pot->strips->fact.fact_size; ++i){
        borISetFree(&pot->fact[i].op_del);
        borISetFree(&pot->fact[i].op_add);
    }
    if (pot->fact != NULL)
        BOR_FREE(pot->fact);

    pddlStripsDel(pot->strips);
}

void pot(const pddl_strips_t *strips)
{
    pot_t pot;

    potInit(&pot, strips);
    potFree(&pot);
}


/*** PLAN SYNC PRODUCTS ***/
struct sprod_op {
    bor_iset_t mgroup;
    bor_iset_t op;
    int size;
};
typedef struct sprod_op sprod_op_t;

struct sprod_mgroup {
    bor_iset_t mgroup;
    bor_iset_t op;
    int is_goal;
};
typedef struct sprod_mgroup sprod_mgroup_t;

struct sprod_graph {
    sprod_op_t *op;
    int op_size;
    sprod_mgroup_t *mgroup;
    int mgroup_size;
};
typedef struct sprod_graph sprod_graph_t;

static int sprodGraphMinimizeCmp(const void *a, const void *b, void *ud)
{
    const int id1 = *(int *)a;
    const int id2 = *(int *)b;
    sprod_graph_t *graph = ud;
    const sprod_op_t *op1 = graph->op + id1;
    const sprod_op_t *op2 = graph->op + id2;

    return borISetCmp(&op1->mgroup, &op2->mgroup);
}

static void sprodGraphMinimize(sprod_graph_t *graph)
{
    int cur, prev;
    int *ops, mgi;

    // First remove ops with only one incident mgroup
    for (int i = 0; i < graph->op_size; ++i){
        sprod_op_t *op = graph->op + i;
        if (borISetSize(&op->mgroup) == 1){
            borISetRm(&graph->mgroup[borISetGet(&op->mgroup, 0)].op, i);
            borISetEmpty(&op->mgroup);
            borISetEmpty(&op->op);
            op->size = 0;
        }
    }

    // Sort operators according to the incident mgroups
    ops = BOR_ALLOC_ARR(int, graph->op_size);
    for (int i = 0; i < graph->op_size; ++i)
        ops[i] = i;
    borSort(ops, graph->op_size, sizeof(int), sprodGraphMinimizeCmp, graph);

    // skip empty ops
    for (cur = 0;
            cur < graph->op_size
                && borISetSize(&graph->op[ops[cur]].mgroup) == 0;
         ++cur);

    // join ops that are incident with the same mgroups
    for (prev = cur++; cur < graph->op_size; ++cur){
        sprod_op_t *op_prev = graph->op + ops[prev];
        sprod_op_t *op_cur = graph->op + ops[cur];
        if (borISetEq(&op_prev->mgroup, &op_cur->mgroup)){
            BOR_ISET_FOR_EACH(&op_prev->mgroup, mgi)
                borISetRm(&graph->mgroup[mgi].op, ops[prev]);
            op_cur->size += op_prev->size;
            borISetUnion(&op_cur->op, &op_prev->op);
            borISetEmpty(&op_prev->mgroup);
            borISetEmpty(&op_prev->op);
            op_prev->size = 0;
        }
        prev = cur;
    }

    for (int i = 0; i < graph->op_size; ++i){
        if (graph->op[i].size == 0)
            continue;
        printf("Op %d [%d]:", i, graph->op[i].size);
        int mg;
        BOR_ISET_FOR_EACH(&graph->op[i].mgroup, mg)
            printf(" %d", mg);
        printf(" | ");
        int op;
        BOR_ISET_FOR_EACH(&graph->op[i].op, op)
            printf(" %d", op);
        printf("\n");

    }
    for (int i = 0; i < graph->mgroup_size; ++i){
        printf("MG %d:", i);
        printf(" [");
        int mg;
        BOR_ISET_FOR_EACH(&graph->mgroup[i].mgroup, mg)
            printf(" %d", mg);
        printf(" ]");
        int op;
        BOR_ISET_FOR_EACH(&graph->mgroup[i].op, op)
            printf(" %d", op);
        printf("\n");

    }
    BOR_FREE(ops);
}

static int sprodGraphMergeCmp(const void *a, const void *b, void *ud)
{
    const int id1 = *(int *)a;
    const int id2 = *(int *)b;
    sprod_graph_t *graph = ud;
    const sprod_op_t *op1 = graph->op + id1;
    const sprod_op_t *op2 = graph->op + id2;

    return op2->size - op1->size;
}


static void sprodGraphGatherMGroups(const sprod_graph_t *graph,
                                    const bor_iset_t *mg,
                                    bor_iset_t *mgroups)
{
    int idx;

    borISetEmpty(mgroups);
    BOR_ISET_FOR_EACH(mg, idx)
        borISetUnion(mgroups, &graph->mgroup[idx].mgroup);
}

static int sprodMGroupsHasGoal(const pot_t *pot, const bor_iset_t *mgroups)
{
    int mi;

    BOR_ISET_FOR_EACH(mgroups, mi){
        if (pot->mgroup[mi].is_goal)
            return 1;
    }
    return 0;
}

static int sprodGraphMergeMGroups(const pot_t *pot,
                                   sprod_graph_t *graph,
                                   const bor_iset_t *mgs)
{
    int dst_id = borISetGet(mgs, 0);
    sprod_mgroup_t *dst = graph->mgroup + dst_id;
    unsigned long num_nodes;

    if (!sprodMGroupsHasGoal(pot, mgs))
        return -1;

    // TODO: parametrize
    num_nodes = tgNumNodes(pot, mgs);
    if (!tgCanFitInMem(num_nodes, borISetSize(mgs),
                       6UL * 1024UL * 1024UL * 1024UL)){
        return -1;
    }

    for (int i = 1; i < mgs->size; ++i){
        int src_id = borISetGet(mgs, i);
        sprod_mgroup_t *src = graph->mgroup + src_id;
        borISetUnion(&dst->mgroup, &src->mgroup);
        borISetUnion(&dst->op, &src->op);
        dst->is_goal |= src->is_goal;

        int opi;
        BOR_ISET_FOR_EACH(&src->op, opi){
            borISetRm(&graph->op[opi].mgroup, src_id);
            borISetAdd(&graph->op[opi].mgroup, dst_id);
        }

        borISetEmpty(&src->mgroup);
        borISetEmpty(&src->op);
    }

    return 0;
}

static int sprodGraphMerge(const pot_t *pot, sprod_graph_t *graph)
{
    BOR_ISET(mgroups);
    int *ops;

    sprodGraphMinimize(graph);

    // Sort operators according to the incident mgroups
    ops = BOR_ALLOC_ARR(int, graph->op_size);
    for (int i = 0; i < graph->op_size; ++i)
        ops[i] = i;
    borSort(ops, graph->op_size, sizeof(int), sprodGraphMergeCmp, graph);

    for (int i = 0; i < graph->op_size; ++i){
        if (graph->op[ops[i]].size == 0)
            break;
        sprodGraphGatherMGroups(graph, &graph->op[ops[i]].mgroup, &mgroups);
        if (sprodGraphMergeMGroups(pot, graph, &mgroups) == 0){
            BOR_FREE(ops);
            borISetFree(&mgroups);
            return 0;
        }
    }

    for (int i = 0; i < graph->op_size; ++i){
        if (graph->op[ops[i]].size == 0)
            break;
        const sprod_op_t *op = graph->op + ops[i];
        for (int j = 0; j < borISetSize(&op->mgroup); ++j){
            int m1 = borISetGet(&op->mgroup, j);
            for (int k = j + 1; k < borISetSize(&op->mgroup); ++k){
                int m2 = borISetGet(&op->mgroup, k);
                borISetUnion2(&mgroups, &graph->mgroup[m1].mgroup,
                                        &graph->mgroup[m2].mgroup);
                if (sprodGraphMergeMGroups(pot, graph, &mgroups) == 0){
                    BOR_FREE(ops);
                    borISetFree(&mgroups);
                    return 0;
                }
            }
        }
    }
    BOR_FREE(ops);

    for (int i = 0; i < graph->mgroup_size; ++i){
        const sprod_mgroup_t *m1 = graph->mgroup + i;
        if (borISetSize(&m1->mgroup) == 0)
            continue;
        for (int j = i + 1; j < graph->mgroup_size; ++j){
            const sprod_mgroup_t *m2 = graph->mgroup + j;
            if (borISetSize(&m2->mgroup) == 0)
                continue;
            borISetUnion2(&mgroups, &m1->mgroup, &m2->mgroup);
            if (sprodGraphMergeMGroups(pot, graph, &mgroups) == 0){
                borISetFree(&mgroups);
                return 0;
            }
        }
    }

    borISetFree(&mgroups);
    return -1;
}

static void sprodGraphInit(const pot_t *pot, sprod_graph_t *graph)
{
    int op;

    // Create a bipartite graph, one side consisting of nodes representing
    // operators and on the other side a node for each mgroup.
    graph->op_size = pot->strips->op.op_size;
    graph->op = BOR_CALLOC_ARR(sprod_op_t, graph->op_size);
    graph->mgroup_size = pot->mgroup_size;
    graph->mgroup = BOR_CALLOC_ARR(sprod_mgroup_t, graph->mgroup_size);

    for (int i = 0; i < pot->strips->op.op_size; ++i){
        graph->op[i].size = 1;
        borISetAdd(&graph->op[i].op, i);
    }

    // Connect operators and mgroups
    for (int i = 0; i < pot->mgroup_size; ++i){
        const mgroup_t *mg = pot->mgroup + i;
        borISetAdd(&graph->mgroup[i].mgroup, i);
        borISetUnion(&graph->mgroup[i].op, &mg->op);
        graph->mgroup[i].is_goal = mg->is_goal;

        BOR_ISET_FOR_EACH(&mg->op, op)
            borISetAdd(&graph->op[op].mgroup, i);
    }
}

static void sprodGraphFree(sprod_graph_t *graph)
{
    for (int i = 0; i < graph->op_size; ++i)
        borISetFree(&graph->op[i].mgroup);
    if (graph->op != NULL)
        BOR_FREE(graph->op);

    for (int i = 0; i < graph->mgroup_size; ++i){
        borISetFree(&graph->mgroup[i].mgroup);
        borISetFree(&graph->mgroup[i].op);
    }
    if (graph->mgroup != NULL)
        BOR_FREE(graph->mgroup);
}

static void sprodCostPart(pot_t *pot, const sprod_graph_t *graph)
{
    int scale = 1;

    // Compute scaling factor for operators' costs
    for (int opi = 0; opi < graph->op_size; ++opi){
        int size = borISetSize(&graph->op[opi].mgroup);
        if (size > 1)
            scale = pddlLCM(scale, size);
    }
    printf("SCALE: %d\n", scale);

    // Scale operators' costs
    for (int opi = 0; opi < pot->strips->op.op_size; ++opi)
        pot->strips->op.op[opi]->cost *= scale;

    // Cost partitioning of the shared operators
    for (int i = 0; i < graph->op_size; ++i){
        if (graph->op[i].size == 0)
            continue;
        int div = borISetSize(&graph->op[i].mgroup);
        int opi;
        BOR_ISET_FOR_EACH(&graph->op[i].op, opi)
            pot->strips->op.op[opi]->cost /= div;
    }
}

static void sprodMakeList(pot_t *pot, const sprod_graph_t *graph)
{
    int size = 0;

    for (int i = 0; i < graph->mgroup_size; ++i){
        const sprod_mgroup_t *mg = graph->mgroup + i;
        if (!mg->is_goal || borISetSize(&mg->mgroup) == 0)
            continue;
        ++size;
    }

    pot->sync_prod_size = size;
    pot->sync_prod = BOR_CALLOC_ARR(bor_iset_t, size);
    for (int i = 0, j = 0; i < graph->mgroup_size; ++i){
        const sprod_mgroup_t *mg = graph->mgroup + i;
        if (!mg->is_goal || borISetSize(&mg->mgroup) == 0)
            continue;
        borISetUnion(pot->sync_prod + j, &mg->mgroup);
        ++j;
    }
}

static void sprodPrepare(pot_t *pot)
{
    sprod_graph_t graph;
    sprodGraphInit(pot, &graph);
    while (sprodGraphMerge(pot, &graph) == 0);

    // Make cost partitioning of the operators that are shared between
    // mgroups
    sprodCostPart(pot, &graph);

    // Create a list of the planned synchronized products
    sprodMakeList(pot, &graph);

    sprodGraphFree(&graph);
}
