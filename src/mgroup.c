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
#include <boruvka/sort.h>
#include <boruvka/lp.h>
#include "pddl/mgroup.h"
#include "pddl/strips.h"
#include "err.h"
#include "assert.h"


static int mgroupCmp(const void *a, const void *b, void *_)
{
    const pddl_mgroup_t *m1 = a;
    const pddl_mgroup_t *m2 = b;
    int cmp = borISetSize(&m2->fact) - borISetSize(&m1->fact);

    if (cmp == 0)
        return borISetCmp(&m1->fact, &m2->fact);
    return cmp;
}

static void mgroupsSort(pddl_mgroups_t *mgs)
{
    borSort(mgs->mgroup, mgs->mgroup_size, sizeof(pddl_mgroup_t),
            mgroupCmp, NULL);
}

void pddlMGroupInit(pddl_mgroup_t *mg)
{
    bzero(mg, sizeof(*mg));
    borISetInit(&mg->fact);
    mg->none_of_those = -1;
}

void pddlMGroupFree(pddl_mgroup_t *mg)
{
    borISetFree(&mg->fact);
}

void pddlMGroupTGInit(pddl_g_t *tg,
                      const pddl_mgroup_t *mg,
                      const pddl_strips_t *strips)
{
    int fact_id;
    int empty_node = -1, from_node, to_node;
    int *fact_to_node;
    bor_iset_t predel, add;

    fact_to_node = BOR_ALLOC_ARR(int, strips->fact.fact_size);
    pddlGInit(tg);
    BOR_ISET_FOR_EACH(&mg->fact, fact_id){
        fact_to_node[fact_id] = pddlGAddNode(tg);
        pddlGNodeAddLabel(tg, fact_to_node[fact_id], fact_id);
    }

    borISetInit(&predel);
    borISetInit(&add);
    for (int opi = 0; opi < strips->op.op_size; ++opi){
        const pddl_strips_op_t *op = strips->op.op[opi];
        borISetIntersect2(&predel, &mg->fact, &op->pre);
        borISetIntersect(&predel, &op->del_eff);
        ASSERT(borISetSize(&predel) <= 1);

        borISetIntersect2(&add, &mg->fact, &op->add_eff);
        ASSERT(borISetSize(&add) <= 1);
        if (borISetSize(&predel) == 0 && borISetSize(&add) == 0)
            continue;

        if (borISetSize(&predel) > 0){
            from_node = fact_to_node[borISetGet(&predel, 0)];
        }else{
            if (empty_node == -1)
                empty_node = pddlGAddNode(tg);
            from_node = empty_node;
        }

        if (borISetSize(&add) > 0){
            to_node = fact_to_node[borISetGet(&add, 0)];
        }else{
            if (empty_node == -1)
                empty_node = pddlGAddNode(tg);
            to_node = empty_node;
        }
        ASSERT(from_node != to_node);

        pddlGAddOrUpdateEdge(tg, from_node, to_node, opi);
    }
    borISetFree(&add);
    borISetFree(&predel);

    BOR_FREE(fact_to_node);
}

static int tgSyncProductPreIsMutex(const pddl_strips_t *strips,
                                   int op,
                                   const pddl_g_t *tg2,
                                   int n2,
                                   bor_iset_t *set)
{
    borISetUnion2(set, &strips->op.op[op]->pre, &tg2->node[n2].label);
    if (pddlMutexesIsMutex(&strips->mutex, set))
        return 1;
    return 0;
}

static int tgSyncProductEffIsMutex(const pddl_strips_t *strips,
                                   int op,
                                   const pddl_g_t *tg2,
                                   int n2,
                                   bor_iset_t *set)
{
    borISetUnion2(set, &tg2->node[n2].label, &strips->op.op[op]->pre);
    borISetMinus(set, &strips->op.op[op]->del_eff);
    borISetUnion(set, &strips->op.op[op]->add_eff);
    if (pddlMutexesIsMutex(&strips->mutex, set))
        return 1;
    return 0;
}

static void tgSyncProductNode(pddl_g_t *prod,
                              const pddl_g_t *tg1,
                              const pddl_g_t *tg2,
                              const pddl_strips_t *strips,
                              const int *prod_node,
                              int n1,
                              int n2,
                              int node)
{
    BOR_ISET(edge_op);
    BOR_ISET(edge_remain);
    BOR_ISET(facts);
    BOR_ISET(n1_ops);
    int edge1, edge2, pedge, pnode, op;

    BOR_ISET_FOR_EACH(&tg1->node[n1].out, edge1){
        const pddl_g_edge_t *e1 = tg1->edge + edge1;
        borISetSet(&edge_remain, &e1->label);
        borISetUnion(&n1_ops, &e1->label);
        BOR_ISET_FOR_EACH(&tg2->node[n2].out, edge2){
            const pddl_g_edge_t *e2 = tg2->edge + edge2;

            // Consider only operators that emanating from both n1 in tg1
            // and from n2 in tg2. Store those operators in edge_op.
            borISetIntersect2(&edge_op, &e1->label, &e2->label);
            if (borISetSize(&edge_op) == 0)
                continue;

            // Update the set of remaining operators
            borISetMinus(&edge_remain, &edge_op);

            // Determine whether the target node in prod is not mutex
            pnode = prod_node[e1->to * tg2->node_size + e2->to];
            if (pnode == -1)
                continue;

            // Add the actual edge
            pedge = pddlGGetOrAddEdge(prod, node, pnode);
            borISetUnion(&prod->edge[pedge].label, &edge_op);
        }

        // Find out the operators that were not used -- these operators
        // should go from node (n1+n2) to all nodes in prod constructed
        // from n1 (n1+x for all x in tg2). But only if:
        //   1. the precondition of the operator is not mutex with n1+n2, and
        //   2. the effect of the operator is not mutex with the node from tg2
        BOR_ISET_FOR_EACH(&edge_remain, op){
            pnode = prod_node[e1->to * tg2->node_size + n2];
            if (pnode == -1)
                continue;

            // Determine whether the precondition is mutex with n2
            if (tgSyncProductPreIsMutex(strips, op, tg2, n2, &facts))
                continue;

            // Add edge only if the effect is not mutex
            if (!tgSyncProductEffIsMutex(strips, op, tg2, n2, &facts))
                pddlGAddOrUpdateEdge(prod, node, pnode, op);
        }
    }

    // Add edges corresponding to the operators incidenting with n2 but not
    // with n1.
    BOR_ISET_FOR_EACH(&tg2->node[n2].out, edge2){
        const pddl_g_edge_t *e2 = tg2->edge + edge2;
        borISetMinus2(&edge_remain, &e2->label, &n1_ops);

        BOR_ISET_FOR_EACH(&edge_remain, op){
            pnode = prod_node[n1 * tg2->node_size + e2->to];
            if (pnode == -1)
                continue;

            // Determine whether the precondition is mutex with n2
            if (tgSyncProductPreIsMutex(strips, op, tg1, n1, &facts))
                continue;

            // Add edge only if the effect is not mutex
            if (!tgSyncProductEffIsMutex(strips, op, tg1, n1, &facts))
                pddlGAddOrUpdateEdge(prod, node, pnode, op);
        }
    }

    borISetFree(&edge_op);
    borISetFree(&edge_remain);
    borISetFree(&facts);
    borISetFree(&n1_ops);
}

void pddlMGroupTGSyncProduct(pddl_g_t *prod,
                             const pddl_g_t *tg1, const pddl_g_t *tg2,
                             const pddl_strips_t *strips)
{
    BOR_ISET(facts);
    int node, *prod_node;

    pddlGInit(prod);
    prod_node = BOR_ALLOC_ARR(int, tg1->node_size * tg2->node_size);
    for (int i = 0; i < tg1->node_size * tg2->node_size; ++i)
        prod_node[i] = -1;

    // Create nodes in the product transition graph
    for (int n1 = 0; n1 < tg1->node_size; ++n1){
        for (int n2 = 0; n2 < tg2->node_size; ++n2){
            borISetUnion2(&facts, &tg1->node[n1].label,
                                  &tg2->node[n2].label);
            if (pddlMutexesIsMutex(&strips->mutex, &facts))
                continue;
            node = pddlGAddNode(prod);
            borISetUnion(&prod->node[node].label, &tg1->node[n1].label);
            borISetUnion(&prod->node[node].label, &tg2->node[n2].label);
            prod_node[n1 * tg2->node_size + n2] = node;
        }
    }

    // Create edges
    for (int n1 = 0; n1 < tg1->node_size; ++n1){
        for (int n2 = 0; n2 < tg2->node_size; ++n2){
            node = prod_node[n1 * tg2->node_size + n2];
            if (node == -1)
                continue;
            tgSyncProductNode(prod, tg1, tg2, strips, prod_node, n1, n2, node);
        }
    }

    BOR_FREE(prod_node);
    borISetFree(&facts);
}

void pddlMGroupsInit(pddl_mgroups_t *mgs)
{
    bzero(mgs, sizeof(*mgs));
}

void pddlMGroupsFree(pddl_mgroups_t *mgs)
{
    pddl_mgroup_t *m;
    PDDL_MGROUPS_FOR_EACH(mgs, m)
        pddlMGroupFree(m);
    if (mgs->mgroup != NULL)
        BOR_FREE(mgs->mgroup);
}

pddl_mgroups_t *pddlMGroupsNew(void)
{
    pddl_mgroups_t *mgs;
    mgs = BOR_ALLOC(pddl_mgroups_t);
    pddlMGroupsInit(mgs);
    return mgs;
}

void pddlMGroupsDel(pddl_mgroups_t *mgs)
{
    pddlMGroupsFree(mgs);
    BOR_FREE(mgs);
}

void pddlMGroupsCopy(pddl_mgroups_t *dst, const pddl_mgroups_t *src)
{
    const pddl_mgroup_t *sm;
    pddl_mgroup_t *dm;

    PDDL_MGROUPS_FOR_EACH(src, sm){
        dm = pddlMGroupsAdd(dst, &sm->fact);
        dm->none_of_those = sm->none_of_those;
        dm->is_init = sm->is_init;
        dm->is_goal = sm->is_goal;
        dm->is_fa = sm->is_fa;
        dm->is_exactly_1 = sm->is_exactly_1;
    }
}

pddl_mgroup_t *pddlMGroupsAdd(pddl_mgroups_t *mgs, const bor_iset_t *mg)
{
    pddl_mgroup_t *g;

    if (mgs->mgroup_size >= mgs->mgroup_alloc){
        if (mgs->mgroup_alloc == 0)
            mgs->mgroup_alloc = 1;
        mgs->mgroup_alloc *= 2;
        mgs->mgroup = BOR_REALLOC_ARR(mgs->mgroup, pddl_mgroup_t,
                                      mgs->mgroup_alloc);
    }

    g = mgs->mgroup + mgs->mgroup_size++;
    pddlMGroupInit(g);
    borISetUnion(&g->fact, mg);
    return g;
}


int pddlMGroupsFA(const pddl_strips_t *strips, pddl_mgroups_t *mgs)
{
    pddl_mgroup_t *mg;
    bor_lp_t *lp;
    unsigned lp_flags;
    bor_iset_t predel, fa_mgroup;
    int rows, fact;
    double val, *obj;

    if (!borLPSolverAvailable(BOR_LP_DEFAULT)){
        ERR_RET2(-1, "Cannot compute fam-groups, because ILP solver is not"
                     " avaiable.");
    }

    if (strips->has_cond_eff){
        ERR_RET2(-1, "Cannot compute fam-groups on problems with conditional"
                     " effects. (They can be compiled away.)");
    }

    lp_flags  = BOR_LP_DEFAULT;
    lp_flags |= BOR_LP_NUM_THREADS(1); // TODO: Parametrize
    lp_flags |= BOR_LP_MAX;
    rows = strips->op.op_size + 1;
    lp = borLPNew(rows, strips->fact.fact_size, lp_flags);

    // Set up coeficients in the objective function and set up binary
    // variables
    for (int i = 0; i < strips->fact.fact_size; ++i){
        borLPSetObj(lp, i, 1.);
        borLPSetVarBinary(lp, i);
    }

    // Initial state constraintf
    BOR_ISET_FOR_EACH(&strips->init, fact)
        borLPSetCoef(lp, 0, fact, 1.);
    borLPSetRHS(lp, 0, 1., 'L');

    // Operator constraints
    borISetInit(&predel);
    for (int oi = 0; oi < strips->op.op_size; ++oi){
        const pddl_strips_op_t *op = strips->op.op[oi];
        BOR_ISET_FOR_EACH(&op->add_eff, fact)
            borLPSetCoef(lp, oi + 1, fact, 1.);

        borISetEmpty(&predel);
        borISetUnion(&predel, &op->pre);
        borISetIntersect(&predel, &op->del_eff);
        BOR_ISET_FOR_EACH(&predel, fact)
            borLPSetCoef(lp, oi + 1, fact, -1.);
        borLPSetRHS(lp, oi + 1, 0., 'L');
    }
    borISetFree(&predel);

    borISetInit(&fa_mgroup);
    obj = BOR_ALLOC_ARR(double, strips->fact.fact_size);
    while (borLPSolve(lp, &val, obj) == 0 && val > 0.5){
        double rhs = 1.;
        char sense = 'G';
        borLPAddRows(lp, 1, &rhs, &sense);
        borISetEmpty(&fa_mgroup);
        for (int i = 0; i < strips->fact.fact_size; ++i){
            if (obj[i] < 0.5){
                borLPSetCoef(lp, rows, i, 1.);
            }else{
                borISetAdd(&fa_mgroup, i);
            }
        }
        mg = pddlMGroupsAdd(mgs, &fa_mgroup);
        mg->is_fa = 1;
        if (borISetIntersectionSizeAtLeast(&mg->fact, &strips->init, 1))
            mg->is_init = 1;
        if (borISetIntersectionSizeAtLeast(&mg->fact, &strips->goal, 1))
            mg->is_goal = mg->is_exactly_1 = 1;
        ++rows;
    }
    BOR_FREE(obj);
    borISetFree(&fa_mgroup);

    borLPDel(lp);

    return 0;
}

void pddlMGroupsFinalize(pddl_mgroups_t *mgs, const pddl_strips_t *strips)
{
    pddl_mgroup_t *mg;

    mgroupsSort(mgs);
    PDDL_MGROUPS_FOR_EACH(mgs, mg){
        if (mg->is_fa && mg->is_goal){
            mg->is_exactly_1 = 1;
            continue;
        }
        mg->is_exactly_1 = 1;
        for (int opi = 0; opi < strips->op.op_size; ++opi){
            const pddl_strips_op_t *op = strips->op.op[opi];
            if (borISetIsDisjunct(&op->add_eff, &mg->fact)
                    && !borISetIsDisjunct(&op->del_eff, &mg->fact)){
                mg->is_exactly_1 = 0;
                break;
            }
        }
    }
}

void pddlMGroupsPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                            const pddl_mgroups_t *ms, FILE *fout)
{
    const pddl_mgroup_t *m;
    int fact;

    if (ms->mgroup_size == 0)
        return;

    for (int i = 0; i < ms->mgroup_size; ++i){
        m = ms->mgroup + i;
        if (m->none_of_those >= 0)
            fprintf(fout, "n:");
        if (m->is_init)
            fprintf(fout, "i:");
        if (m->is_goal)
            fprintf(fout, "g:");
        if (m->is_fa)
            fprintf(fout, "fa:");
        if (m->is_exactly_1)
            fprintf(fout, "e1:");
        fprintf(fout, "%d ::", borISetSize(&m->fact));
        BOR_ISET_FOR_EACH(&m->fact, fact)
            fprintf(fout, " %s;", fs->fact[fact]->name);
        fprintf(fout, "\n");
    }
}

void pddlMGroupsPrintPython(const pddl_mgroups_t *mg, FILE *fout)
{
    const pddl_mgroup_t *g;

    fprintf(fout, "[\n");
    PDDL_MGROUPS_FOR_EACH(mg, g){
        int fact_id;

        fprintf(fout, "    {\n");

        fprintf(fout, "        'fact' : set([");
        BOR_ISET_FOR_EACH(&g->fact, fact_id)
            fprintf(fout, " %d,", fact_id);
        fprintf(fout, "]),\n");
        if (g->none_of_those < 0){
            fprintf(fout, "        'none_of_those' : None,\n");
        }else{
            fprintf(fout, "        'none_of_those' : %d,\n",
                    g->none_of_those);
        }
        fprintf(fout, "        'is_init' : %s,\n",
                (g->is_init ? "True" : "False"));
        fprintf(fout, "        'is_goal' : %s,\n",
                (g->is_goal ? "True" : "False"));
        fprintf(fout, "        'is_fa' : %s,\n",
                (g->is_fa ? "True" : "False"));
        fprintf(fout, "        'is_exactly_1' : %s,\n",
                (g->is_exactly_1 ? "True" : "False"));

        fprintf(fout, "    },\n");
    }
    fprintf(fout, "]\n");
}
