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
    int depth;
    int leaf;
    pddl_obj_id_t obj;
    pred_tnode_t *child;
    int child_size;
    int child_alloc;
    bor_iset_t fact;
};

struct pred_tree {
    int arg_size;
    int *arg;
    int *param;
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
    borISetFree(&tnode->fact);
}

static void predTreeInitNode(pred_tree_t *tree,
                             pred_tnode_t *tnode,
                             int next,
                             pddl_obj_id_t obj)
{
    bzero(tnode, sizeof(*tnode));
    tnode->depth = next;
    tnode->obj = obj;
    if (next >= tree->arg_size){
        tnode->leaf = 1;
    }else{
        tnode->leaf = 0;
    }
}

struct arg_order {
    const pddl_cond_atom_t *atom;
    const pddl_params_t *mg_params;
};

static int cmpArgOrder(const void *a, const void *b, void *_ao)
{
    int ai1 = *(int *)a;
    int ai2 = *(int *)b;
    const struct arg_order *ao = _ao;
    const pddl_cond_atom_t *atom = ao->atom;
    const pddl_params_t *params = ao->mg_params;

    int a1obj = atom->arg[ai1].obj;
    int a1param = atom->arg[ai1].param;
    int a2obj = atom->arg[ai2].obj;
    int a2param = atom->arg[ai2].param;
    if (a1obj >= 0 && a2obj >= 0){
        return a1obj - a2obj;
    }else if (a1obj >= 0){
        return 1;
    }else if (a2obj >= 0){
        return -1;
    }else{
        if (params->param[a1param].is_counted_var
                == params->param[a2param].is_counted_var){
            return a1param - a2param;
        }else if (params->param[a1param].is_counted_var){
            return 1;
        }else{
            return -1;
        }
    }

}

static void predTreeInit(pred_tree_t *tree,
                         const pddl_lifted_mgroup_t *mg,
                         const pddl_cond_atom_t *atom)
{
    bzero(tree, sizeof(*tree));
    tree->arg_size = atom->arg_size;
    tree->arg = BOR_ALLOC_ARR(int, atom->arg_size);

    for (int i = 0; i < tree->arg_size; ++i)
        tree->arg[i] = i;
    struct arg_order ao = { atom, &mg->param };
    borSort(tree->arg, tree->arg_size, sizeof(int), cmpArgOrder, &ao);

    int arg_size = 0;
    for (; arg_size < tree->arg_size; ++arg_size){
        int argi = tree->arg[arg_size];
        if (atom->arg[argi].obj >= 0
                || mg->param.param[atom->arg[argi].param].is_counted_var){
            break;
        }
    }
    tree->arg_size = arg_size;

    tree->param = BOR_ALLOC_ARR(int, tree->arg_size);
    for (int i = 0; i < tree->arg_size; ++i){
        tree->param[i] = atom->arg[tree->arg[i]].param;
    }

    predTreeInitNode(tree, &tree->root, 0, -1);
}

static void predTreeFree(pred_tree_t *tree)
{
    if (tree->arg != NULL)
        BOR_FREE(tree->arg);
    if (tree->param != NULL)
        BOR_FREE(tree->param);
    predTNodeFree(&tree->root);
}

static void _predTreeAdd(pred_tree_t *tree,
                         pred_tnode_t *tnode,
                         const pddl_fact_t *fact)
{
    if (tnode->leaf){
        borISetAdd(&tnode->fact, fact->id);
    }else{
        int argi = tree->arg[tnode->depth];
        pddl_obj_id_t fact_obj = fact->ground_atom->arg[argi];

        for (int i = 0; i < tnode->child_size; ++i){
            if (tnode->child[i].obj == fact_obj){
                _predTreeAdd(tree, tnode->child + i, fact);
                return;
            }
        }

        if (tnode->child_alloc == tnode->child_size){
            if (tnode->child_alloc == 0)
                tnode->child_alloc = 2;
            tnode->child_alloc *= 2;
            tnode->child = BOR_REALLOC_ARR(tnode->child, pred_tnode_t,
                                           tnode->child_alloc);
        }

        pred_tnode_t *next = tnode->child + tnode->child_size++;
        predTreeInitNode(tree, next, tnode->depth + 1, fact_obj);
        _predTreeAdd(tree, next, fact);
    }
}

static void predTreeAdd(pred_tree_t *tree, const pddl_fact_t *fact)
{
    _predTreeAdd(tree, &tree->root, fact);
}


static void buildPredTrees(pred_tree_t *tree,
                           const pddl_t *pddl,
                           const pddl_strips_t *strips,
                           const pddl_lifted_mgroup_t *mg)
{
    bzero(tree, sizeof(*tree) * mg->cond.size);
    for (int ci = 0; ci < mg->cond.size; ++ci){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(mg->cond.cond[ci], atom);
        predTreeInit(tree + ci, mg, a);
    }

    const pddl_fact_t *fact;
    PDDL_FACTS_FOR_EACH(&strips->fact, fact){
        if (fact->ground_atom == NULL)
            continue;
        const pddl_ground_atom_t *ga = fact->ground_atom;
        for (int ci = 0; ci < mg->cond.size; ++ci){
            const pddl_cond_atom_t *a = PDDL_COND_CAST(mg->cond.cond[ci], atom);
            if (a->pred == ga->pred && checkFact(&pddl->type, ga, mg, a))
                predTreeAdd(&tree[ci], fact);
        }
    }
}

static void _gen(pred_tree_t *tree,
                 pred_tnode_t **tnode,
                 int tree_size,
                 int param_i,
                 const bor_iset_t *params,
                 int lifted_mgroup_id,
                 pddl_ground_mgroups_t *mg)
{
    if (param_i == borISetSize(params)){
        // We have fixed all variables -- create the mutex group from all
        // leaf nodes
        BOR_ISET(mgroup);
        for (int i = 0; i < tree_size; ++i){
            if (tnode[i]->leaf)
                borISetUnion(&mgroup, &tnode[i]->fact);
        }
        if (borISetSize(&mgroup) > 0)
            pddlGroundMGroupsAdd(mg, &mgroup, lifted_mgroup_id);
        borISetFree(&mgroup);
        return;
    }

    // Find all nodes corresponding to the current parameter and collect
    // all possible objects that can be bound to this parameter
    int param = borISetGet(params, param_i);
    BOR_ISET(relevant_tree);
    BOR_ISET(param_obj);
    for (int i = 0; i < tree_size; ++i){
        if (tnode[i]->depth < tree[i].arg_size
                && tree[i].param[tnode[i]->depth] == param){
            borISetAdd(&relevant_tree, i);
            for (int ci = 0; ci < tnode[i]->child_size; ++ci)
                borISetAdd(&param_obj, tnode[i]->child[ci].obj);
        }
    }

    // Prepare an array of tree nodes for recursive descent
    pred_tnode_t *next_tnode[tree_size];
    int obj;
    BOR_ISET_FOR_EACH(&param_obj, obj){
        // For each object find nodes that match and descent in those nodes
        memcpy(next_tnode, tnode, sizeof(pred_tnode_t *) * tree_size);
        int tree_i;
        BOR_ISET_FOR_EACH(&relevant_tree, tree_i){
            for (int ci = 0; ci < tnode[tree_i]->child_size; ++ci){
                pred_tnode_t *ch = tnode[tree_i]->child + ci;
                if (ch->obj == obj)
                    next_tnode[tree_i] = ch;
            }
        }

        // All trees are matched, we can descent for the current
        // combination of objects
        // Note that at least one next_tnode[] must be changed because we
        // collected objects into param_obj in that way
        _gen(tree, next_tnode, tree_size, param_i + 1, params,
             lifted_mgroup_id, mg);
    }

    borISetFree(&relevant_tree);
    borISetFree(&param_obj);
}

static void gen(pred_tree_t *tree, int tree_size,
                int lifted_mgroup_id, pddl_ground_mgroups_t *mg)
{
    BOR_ISET(param);
    for (int i = 0; i < tree_size; ++i){
        for (int j = 0; j < tree[i].arg_size; ++j){
            borISetAdd(&param, tree[i].param[j]);
        }
    }

    pred_tnode_t *tnode[tree_size];
    for (int i = 0; i < tree_size; ++i)
        tnode[i] = &tree[i].root;

    _gen(tree, tnode, tree_size, 0, &param, lifted_mgroup_id, mg);

    borISetFree(&param);
}


/** DEBUG
static void predTNodePrint(pred_tnode_t *tn,
                           const pddl_strips_t *strips,
                           FILE *fout)
{
    for (int i = 0; i < tn->depth; ++i)
        fprintf(fout, "  ");
    fprintf(fout, "%d:%d:%d ::", tn->obj, tn->depth, tn->leaf);
    int fact;
    BOR_ISET_FOR_EACH(&tn->fact, fact){
        fprintf(fout, " (%s)", strips->fact.fact[fact]->name);
    }
    fprintf(fout, "\n");

    for (int i = 0; i < tn->child_size; ++i){
        predTNodePrint(tn->child + i, strips, fout);
    }
}

static void predTreePrint(pred_tree_t *tree,
                          const pddl_strips_t *strips,
                          FILE *fout)
{
    fprintf(fout, "arg:");
    for (int i = 0; i < tree->arg_size; ++i)
        fprintf(fout, " %d", tree->arg[i]);
    fprintf(fout, ", param:");
    for (int i = 0; i < tree->arg_size; ++i)
        fprintf(fout, " %d", tree->param[i]);
    fprintf(fout, "\n");
    predTNodePrint(&tree->root, strips, fout);
}
*/


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

    /** DEBUG
    for (int i = 0; i < lifted_mg->cond.size; ++i){
        fprintf(stderr, "[[%d]]: ", i);
        pddlLiftedMGroupPrint(pddl, lifted_mg, stderr);
        predTreePrint(tree + i, strips, stderr);
    }
    fprintf(stderr, "\n");
    */

    gen(tree, lifted_mg->cond.size, lifted_mg_id, mg);

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
    if (cmp == 0)
        cmp = m1->lifted_mgroup_id - m2->lifted_mgroup_id;
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
    BOR_ISET(lmgs);
    for (int i = 0; i < mg->mgroup_size; ++i){
        const pddl_ground_mgroup_t *m = mg->mgroup + i;
        borISetAdd(&lmgs, m->lifted_mgroup_id);
    }

    int lmgid;
    BOR_ISET_FOR_EACH(&lmgs, lmgid){
        if (lmgid >= 0){
            pddlLiftedMGroupPrint(pddl, mg->lifted_mgroup.mgroup + lmgid, fout);
        }

        for (int i = 0; i < mg->mgroup_size; ++i){
            const pddl_ground_mgroup_t *m = mg->mgroup + i;
            if (m->lifted_mgroup_id != lmgid)
                continue;
            fprintf(fout, "[%d]:", i);
            int fact;
            BOR_ISET_FOR_EACH(&m->mgroup, fact){
                fprintf(fout, " (%s)", strips->fact.fact[fact]->name);
            }
            fprintf(fout, "\n");
        }
        fprintf(fout, "\n");
    }
}
