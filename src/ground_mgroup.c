/***
 * cpddl
 * -------
 * Copyright (c)2019 Daniel Fiser <danfis@danfis.cz>,
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
#include "pddl/pddl_struct.h"
#include "pddl/strips.h"
#include "pddl/ground_mgroup.h"
#include "assert.h"

void pddlGroundMGroupsAdd(pddl_ground_mgroups_t *mg,
                          const bor_iset_t *fact,
                          int lifted_mgroup_id);
void pddlGroundMGroupsSortUniq(pddl_ground_mgroups_t *mg);

typedef struct pred_tnode pred_tnode_t;
struct pred_tnode {
    int argi;
    int mg_param;
    pddl_obj_id_t *child_obj;
    pred_tnode_t *child;
    int child_size;
    int child_alloc;
    bor_iset_t fact;
};

struct pred_tree {
    pred_tnode_t root;
};
typedef struct pred_tree pred_tree_t;


static int checkFact(const pddl_types_t *types,
                     const pddl_ground_atom_t *ga,
                     const pddl_lifted_mgroup_t *mg,
                     const pddl_cond_atom_t *mg_atom)
{
    if (ga->pred != mg_atom->pred)
        return 0;
    ASSERT(ga->arg_size == mg_atom->arg_size);

    for (int ai = 0; ai < ga->arg_size; ++ai){
        if (mg_atom->arg[ai].obj >= 0){
            if (ga->arg[ai] != mg_atom->arg[ai].obj)
                return 0;
        }else{
            int type = mg->param.param[mg_atom->arg[ai].param].type;
            if (!pddlTypesObjHasType(types, type, ga->arg[ai]))
                return 0;
        }
    }

    return 1;
}

static void predTNodeFree(pred_tnode_t *tnode)
{
    for (int i = 0; i < tnode->child_size; ++i)
        predTNodeFree(tnode->child + i);
    if (tnode->child != NULL)
        BOR_FREE(tnode->child);
    if (tnode->child_obj != NULL)
        BOR_FREE(tnode->child_obj);
    borISetFree(&tnode->fact);
}

static void predTreeFree(pred_tree_t *tree)
{
    predTNodeFree(&tree->root);
}

static void treeInitNode(pred_tnode_t *tnode,
                         int prev_argi,
                         const pddl_lifted_mgroup_t *mg,
                         const pddl_cond_atom_t *mg_atom)
{
    bzero(tnode, sizeof(*tnode));
    tnode->argi = -1;
    for (int ai = prev_argi + 1; ai < mg_atom->arg_size; ++ai){
        int atom_param = mg_atom->arg[ai].param;
        if (atom_param >= 0
                && !mg->param.param[atom_param].is_counted_var){
            tnode->argi = ai;
            tnode->mg_param = atom_param;
            return;
        }
    }
}

static void predTreeAdd(pred_tnode_t *tnode,
                        const pddl_fact_t *fact,
                        const pddl_lifted_mgroup_t *mg,
                        const pddl_cond_atom_t *mg_atom)
{
    if (tnode->argi < 0){
        borISetAdd(&tnode->fact, fact->id);
    }else{
        int argi = tnode->argi;
        pddl_obj_id_t fact_obj = fact->ground_atom->arg[argi];
        for (int i = 0; i < tnode->child_size; ++i){
            if (tnode->child_obj[i] == fact_obj){
                predTreeAdd(tnode->child + i, fact, mg, mg_atom);
                return;
            }
        }

        if (tnode->child_alloc == tnode->child_size){
            if (tnode->child_alloc == 0)
                tnode->child_alloc = 2;
            tnode->child_alloc *= 2;
            tnode->child = BOR_REALLOC_ARR(tnode->child, pred_tnode_t,
                                           tnode->child_alloc);
            tnode->child_obj = BOR_REALLOC_ARR(tnode->child_obj, pddl_obj_id_t,
                                               tnode->child_alloc);
        }

        tnode->child_obj[tnode->child_size] = fact_obj;
        pred_tnode_t *next = tnode->child + tnode->child_size++;
        treeInitNode(next, argi, mg, mg_atom);
        predTreeAdd(next, fact, mg, mg_atom);
    }
}

static void buildPredTrees(pred_tree_t *tree,
                           const pddl_t *pddl,
                           const pddl_strips_t *strips,
                           const pddl_lifted_mgroup_t *mg)
{
    bzero(tree, sizeof(*tree) * mg->cond.size);
    for (int ci = 0; ci < mg->cond.size; ++ci){
        bzero(tree + ci, sizeof(*tree));
        const pddl_cond_atom_t *a = PDDL_COND_CAST(mg->cond.cond[ci], atom);
        treeInitNode(&tree[ci].root, -1, mg, a);
    }

    const pddl_fact_t *fact;
    PDDL_FACTS_FOR_EACH(&strips->fact, fact){
        if (fact->ground_atom == NULL)
            continue;
        const pddl_ground_atom_t *ga = fact->ground_atom;
        for (int ci = 0; ci < mg->cond.size; ++ci){
            const pddl_cond_atom_t *a = PDDL_COND_CAST(mg->cond.cond[ci], atom);
            if (a->pred == ga->pred && checkFact(&pddl->type, ga, mg, a))
                predTreeAdd(&tree[ci].root, fact, mg, a);
        }
    }
}

static void predTNodePrint(pred_tnode_t *tn,
                           int obj,
                           int depth,
                           const pddl_strips_t *strips,
                           FILE *fout)
{
    for (int i = 0; i < depth; ++i)
        fprintf(fout, "  ");
    fprintf(fout, "%d:%d ::", obj, tn->argi);
    int fact;
    BOR_ISET_FOR_EACH(&tn->fact, fact){
        fprintf(fout, " (%s)", strips->fact.fact[fact]->name);
    }
    fprintf(fout, "\n");

    for (int i = 0; i < tn->child_size; ++i){
        predTNodePrint(tn->child + i, tn->child_obj[i], depth + 1, strips, fout);
    }
}

static void predTreePrint(pred_tree_t *tree,
                          const pddl_strips_t *strips,
                          FILE *fout)
{
    predTNodePrint(&tree->root, -1, 0, strips, fout);
}


static void _genMGroups(pred_tree_t *tree,
                        int tree_id,
                        pred_tnode_t *tnode,
                        pddl_obj_id_t *mg_arg,
                        const bor_iset_t *fact,
                        const pddl_lifted_mgroup_t *lifted_mg,
                        int lifted_mg_id,
                        pddl_ground_mgroups_t *mg)
{
    if (tnode->argi == -1){
        BOR_ISET(fact_union);
        borISetUnion2(&fact_union, fact, &tnode->fact);

        if (tree_id == lifted_mg->cond.size - 1){
            pddlGroundMGroupsAdd(mg, &fact_union, lifted_mg_id);
        }else{
            _genMGroups(tree, tree_id + 1, &tree[tree_id + 1].root,
                        mg_arg, &fact_union, lifted_mg, lifted_mg_id, mg);
        }
        borISetFree(&fact_union);

    }else{
        pddl_obj_id_t arg = mg_arg[tnode->mg_param];
        for (int ci = 0; ci < tnode->child_size; ++ci){
            if (arg == -1 || arg == tnode->child_obj[ci]){
                mg_arg[tnode->mg_param] = tnode->child_obj[ci];
                _genMGroups(tree, tree_id, tnode->child + ci, mg_arg, fact,
                            lifted_mg, lifted_mg_id, mg);
                mg_arg[tnode->mg_param] = arg;
            }
        }
    }
}

static void genMGroups(pddl_ground_mgroups_t *mg,
                       pred_tree_t *tree,
                       const pddl_lifted_mgroup_t *lifted_mg,
                       int lifted_mg_id)
{
    BOR_ISET(fact);
    pddl_obj_id_t arg[lifted_mg->param.param_size];
    for (int i = 0; i < lifted_mg->param.param_size; ++i)
        arg[i] = -1;

    _genMGroups(tree, 0, &tree[0].root, arg, &fact,
                lifted_mg, lifted_mg_id, mg);
    borISetFree(&fact);
}

static void groundMGroup(pddl_ground_mgroups_t *mg,
                         const pddl_t *pddl,
                         const pddl_strips_t *strips,
                         const pddl_lifted_mgroup_t *lifted_mg,
                         int lifted_mg_id)
{
    if (lifted_mg->cond.size == 0)
        return;

    pred_tree_t tree[lifted_mg->cond.size];
    buildPredTrees(tree, pddl, strips, lifted_mg);

    for (int i = 0; i < lifted_mg->cond.size; ++i){
        fprintf(stderr, "[[%d]]: ", i);
        pddlLiftedMGroupPrint(pddl, lifted_mg, stderr);
        predTreePrint(tree + i, strips, stderr);
    }
    fprintf(stderr, "\n");

    genMGroups(mg, tree, lifted_mg, lifted_mg_id);

    for (int i = 0; i < lifted_mg->cond.size; ++i)
        predTreeFree(tree + i);
}

void pddlGroundMGroupsGround(pddl_ground_mgroups_t *mg,
                             const pddl_t *pddl,
                             const pddl_lifted_mgroups_t *lifted_mg,
                             const pddl_strips_t *strips)
{
    bzero(mg, sizeof(*mg));
    pddlLiftedMGroupsInitCopy(&mg->lifted_mgroup, lifted_mg);

    for (int mgi = 0; mgi < mg->lifted_mgroup.mgroup_size; ++mgi)
        groundMGroup(mg, pddl, strips, mg->lifted_mgroup.mgroup + mgi, mgi);
    pddlGroundMGroupsSortUniq(mg);
}

void pddlGroundMGroupsFree(pddl_ground_mgroups_t *mg)
{
    pddlLiftedMGroupsFree(&mg->lifted_mgroup);
    for (int i = 0; i < mg->mgroup_size; ++i){
        pddl_ground_mgroup_t *m = mg->mgroup + i;
        borISetFree(&m->mgroup);
    }
    if (mg->mgroup != NULL)
        BOR_FREE(mg->mgroup);
}


void pddlGroundMGroupsAdd(pddl_ground_mgroups_t *mg,
                          const bor_iset_t *fact,
                          int lifted_mgroup_id)
{
    if (mg->mgroup_alloc == mg->mgroup_size){
        if (mg->mgroup_alloc == 0)
            mg->mgroup_alloc = 2;
        mg->mgroup_alloc *= 2;
        mg->mgroup = BOR_REALLOC_ARR(mg->mgroup, pddl_ground_mgroup_t,
                                     mg->mgroup_alloc);
    }

    pddl_ground_mgroup_t *m = mg->mgroup + mg->mgroup_size++;
    bzero(m, sizeof(*m));
    borISetUnion(&m->mgroup, fact);
    m->lifted_mgroup_id = lifted_mgroup_id;
}

static int cmpMGroup(const void *a, const void *b, void *_)
{
    const pddl_ground_mgroup_t *m1 = a;
    const pddl_ground_mgroup_t *m2 = b;
    int cmp = borISetSize(&m1->mgroup) - borISetSize(&m2->mgroup);
    if (cmp == 0)
        cmp = borISetCmp(&m1->mgroup, &m2->mgroup);
    return cmp;
}

void pddlGroundMGroupsSortUniq(pddl_ground_mgroups_t *mg)
{
    if (mg->mgroup_size == 0)
        return;

    borSort(mg->mgroup, mg->mgroup_size, sizeof(pddl_ground_mgroup_t),
            cmpMGroup, NULL);

    int ins = 1;
    const pddl_ground_mgroup_t *b = mg->mgroup + 0;
    for (int i = 1; i < mg->mgroup_size; ++i){
        pddl_ground_mgroup_t *m = mg->mgroup + i;
        if (borISetCmp(&b->mgroup, &m->mgroup) == 0){
            borISetFree(&m->mgroup);
        }else{
            if (ins != i)
                mg->mgroup[ins] = mg->mgroup[i];
            b = mg->mgroup + ins;
            ++ins;
        }
    }
    mg->mgroup_size = ins;
}

void pddlGroundMGroupsPrint(const pddl_t *pddl,
                            const pddl_strips_t *strips,
                            const pddl_ground_mgroups_t *mg,
                            FILE *fout)
{
    for (int i = 0; i < mg->mgroup_size; ++i){
        const pddl_ground_mgroup_t *m = mg->mgroup + i;
        fprintf(fout, "[%d]:", i);
        if (m->lifted_mgroup_id >= 0){
            const pddl_lifted_mgroup_t *lm;
            lm = mg->lifted_mgroup.mgroup + m->lifted_mgroup_id;
            pddlLiftedMGroupPrint(pddl, lm, fout);
        }
        fprintf(fout, "  ->");
        int fact;
        BOR_ISET_FOR_EACH(&m->mgroup, fact){
            fprintf(fout, " (%s)", strips->fact.fact[fact]->name);
        }
        fprintf(fout, "\n");
    }
}
