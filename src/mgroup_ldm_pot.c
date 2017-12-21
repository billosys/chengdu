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

#include "pddl/strips.h"
#include "pddl/mgroup_ldm_pot.h"
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
};
typedef struct mgroup mgroup_t;

struct pot {
    pddl_strips_t *strips;
    fact_t *fact;
    mgroup_t *mgroup;
    int mgroup_size;
    int mgroup_alloc;
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

    if (pot->mgroup_size >= pot->mgroup_alloc){
        pot->mgroup_alloc *= 2;
        pot->mgroup = BOR_REALLOC_ARR(pot->mgroup, mgroup_t, pot->mgroup_alloc);
    }

    mg = pot->mgroup + pot->mgroup_size;
    bzero(mg, sizeof(*mg));
    mg->id = pot->mgroup_size;
    borISetAdd(&mg->fact, fact_id);
    mgroupMakeExactlyOne(mg, pot->strips);

    ++pot->mgroup_size;
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
}

static void potFree(pot_t *pot)
{
    pddlStripsDel(pot->strips);

    for (int i = 0; i < pot->strips->fact.fact_size; ++i){
        borISetFree(&pot->fact[i].op_del);
        borISetFree(&pot->fact[i].op_add);
    }
    if (pot->fact != NULL)
        BOR_FREE(pot->fact);
}

void pot(const pddl_strips_t *strips)
{
    pot_t pot;

    potInit(&pot, strips);
    potFree(&pot);
}
