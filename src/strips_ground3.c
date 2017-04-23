/***
 * cpddl
 * -------
 * Copyright (c)2016 Daniel Fiser <danfis@danfis.cz>,
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
#include <boruvka/htable.h>
#include <boruvka/hfunc.h>
#include <boruvka/sort.h>
#include "pddl/strips.h"
#include "pddl/prep_action.h"
#include "err.h"
#include "assert.h"

// TODO: Define mask_t as a structure with functions to set and unset bits
//       to allow more than 64 preconditions.

typedef unsigned char obj_id_t;
#define UNDEF UCHAR_MAX

typedef uint32_t pre_mask_t;


struct ground;

struct tnode_child {
    obj_id_t child_size;
    obj_id_t child_alloc;
    struct tnode **child;
} bor_packed;
typedef struct tnode_child tnode_child_t;

struct tnode_flags {
    unsigned char blocked:1; /*!< True if no new children are allowed */
    unsigned char pre_unified:1; /*!< True if the node unified
                                      a new precondition */
    unsigned char static_arg:1; /*!< True if the node corresponds to an
                                     argument of a static fact */
} bor_packed;
typedef struct tnode_flags tnode_flags_t;

struct tnode {
    obj_id_t obj_id;
    pre_mask_t pre_mask; /*!< Bits set on positions where precondition is set */
    int pre_unified; /*!< Number of unified preconditions */
    tnode_flags_t flags;
    tnode_child_t child[];
} bor_packed;
typedef struct tnode tnode_t;

#define PRE_MASK_SET(N, IDX) \
    ((N)->pre_mask = (N)->pre_mask | (((pre_mask_t)1u) << (pre_mask_t)(IDX)))

struct pred_to_pre {
    int *pre;
    int size;
};
typedef struct pred_to_pre pred_to_pre_t;

struct tree {
    struct ground *g;
    int action_id;
    const pddl_prep_action_t *action;
    int arg_size;
    int *arg_max_size;
    int pre_size;
    pre_mask_t pre_mask;
    pre_mask_t pre_static_mask; /*!< Mask only on static preconditions */
    pred_to_pre_t *pred_to_pre;
    tnode_t *root;
};
typedef struct tree tree_t;

struct ground_args {
    int *arg;
    int action_id;
    const pddl_prep_action_t *action;
    int op_id;
};
typedef struct ground_args ground_args_t;

struct ground_args_arr {
    ground_args_t *arg;
    int size;
    int alloc;
};
typedef struct ground_args_arr ground_args_arr_t;

struct ground {
    const pddl_t *pddl;
    pddl_prep_actions_t action;
    pddl_facts_t static_fact;
    pddl_facts_t fact;
    pddl_strips_ops_t op;
    tree_t *tree;
    ground_args_arr_t ground_args;
};
typedef struct ground ground_t;

static void treePrint(tree_t *tr, FILE *fout);
static void groundActionAddEff(ground_t *g,
                               const pddl_prep_action_t *a,
                               const obj_id_t *oarg);

static void groundArgsFree(ground_args_arr_t *ga)
{
    for (int i = 0; i < ga->size; ++i){
        if (ga->arg[i].arg != NULL)
            BOR_FREE(ga->arg[i].arg);
    }
    if (ga->arg != NULL)
        BOR_FREE(ga->arg);
}
static void groundArgsAdd(ground_args_arr_t *ga, int action_id,
                          const pddl_prep_action_t *action,
                          const int *arg)
{
    ground_args_t *garg;

    if (ga->size >= ga->alloc){
        if (ga->alloc == 0)
            ga->alloc = 4;
        ga->alloc *= 2;
        ga->arg = BOR_REALLOC_ARR(ga->arg, ground_args_t, ga->alloc);
    }

    garg = ga->arg + ga->size++;
    garg->arg = BOR_ALLOC_ARR(int, action->param_size);
    memcpy(garg->arg, arg, sizeof(int) * action->param_size);
    garg->action_id = action_id;
    garg->action = action;
    garg->op_id = -1;
}

static int groundArgsCmp(const void *a, const void *b, void *_)
{
    const ground_args_t *g1 = a;
    const ground_args_t *g2 = b;
    int g1_action_id = g1->action_id;
    int g2_action_id = g2->action_id;
    int cmp;

    if (g1->action->parent_action >= 0)
        g1_action_id = g1->action->parent_action;
    if (g2->action->parent_action >= 0)
        g2_action_id = g2->action->parent_action;

    if (g1_action_id == g2_action_id){
        cmp = memcmp(g1->arg, g2->arg, sizeof(int) * g1->action->param_size);
        if (cmp != 0)
            return cmp;
        if (g1->action->parent_action < 0 && g2->action->parent_action < 0)
            return 0;
        if (g1->action->parent_action < 0)
            return -1;
        if (g2->action->parent_action < 0)
            return 1;
    }
    return g1_action_id - g2_action_id;
}

static void groundArgsSortAndUniq(ground_args_arr_t *ga)
{
    int ins;

    if (ga->arg == 0)
        return;

    borSort(ga->arg, ga->size, sizeof(ground_args_t), groundArgsCmp, NULL);

    ins = 0;
    for (int i = 1; i < ga->size; ++i){
        if (groundArgsCmp(ga->arg + i, ga->arg + ins, NULL) == 0){
            if (ga->arg[i].arg != NULL)
                BOR_FREE(ga->arg[i].arg);
        }else{
            ga->arg[++ins] = ga->arg[i];
        }
    }
    ga->size = ins + 1;
}


_bor_inline int tnodeMaxByteSize(tree_t *tr)
{
    return bor_offsetof(tnode_t, child) + tr->arg_size * sizeof(tnode_child_t);
}

static tnode_t *tnodeNew(tree_t *t, tnode_t *parent, obj_id_t obj_id)
{
    tnode_t *n;
    size_t size;

    size = sizeof(*n) + sizeof(tnode_child_t) * t->arg_size;
    n = BOR_MALLOC(size);
    bzero(n, size);
    n->obj_id = obj_id;
    if (parent != NULL){
        n->pre_mask = parent->pre_mask;
        n->pre_unified = parent->pre_unified;
    }
    return n;
}

static void tnodeDel(tree_t *tr, tnode_t *t);
static void tnodeFree(tree_t *tr, tnode_t *t)
{
    for (int a = 0; a < tr->arg_size; ++a){
        tnode_child_t *cs = t->child + a;
        for (int i = 0; i < cs->child_size; ++i){
            if (cs->child[i] != NULL)
                tnodeDel(tr, cs->child[i]);
        }
        if (cs->child != NULL)
            BOR_FREE(cs->child);
    }
}

static void tnodeDel(tree_t *tr, tnode_t *t)
{
    tnodeFree(tr, t);
    BOR_FREE(t);
}

static void tnodeReserveChild(tree_t *tr, tnode_t *n, int argi)
{
    tnode_child_t *cs;

    cs = n->child + argi;
    if (cs->child_size == cs->child_alloc){
        if (cs->child_alloc == 0)
            cs->child_alloc = 1;
        cs->child_alloc *= 2;
        cs->child = BOR_REALLOC_ARR(cs->child, tnode_t *, cs->child_alloc);
    }
}

static void _tnodeChildBubbleDown(tnode_t *tn, int argi, int idx)
{
    tnode_child_t *cs = tn->child + argi;

    // sort childs according to .obj_id
    for (tnode_t **c = cs->child + idx - 1;
            c >= cs->child && c[0]->obj_id > c[1]->obj_id; --c){
        tnode_t *s = c[0];
        c[0] = c[1];
        c[1] = s;
    }
}

static void tnodeAddChildPtr(tree_t *t, tnode_t *par, int argi, tnode_t *add)
{
    tnode_child_t *cs = par->child + argi;

    tnodeReserveChild(t, par, argi);
    cs->child[cs->child_size++] = add;

    // sort childs according to .obj_id
    _tnodeChildBubbleDown(par, argi, cs->child_size - 1);
}

static tnode_t *tnodeAddChild(tree_t *t, tnode_t *par, int argi, obj_id_t obj_id)
{
    tnode_t *n = tnodeNew(t, par, obj_id);
    tnodeAddChildPtr(t, par, argi, n);
    return n;
}

static void tnodeDelChild(tree_t *tr, tnode_t *par, tnode_t *ch, int argi)
{
    tnode_child_t *cs = par->child + argi;
    int i;

    // TODO: binary search?
    for (i = 0; i < cs->child_size; ++i){
        if (cs->child[i] == ch){
            tnodeDel(tr, ch);
            break;
        }
    }
    for (++i; i < cs->child_size; ++i)
        cs->child[i - 1] = cs->child[i];
    cs->child_size = i - 1;
}

static void propagatePre(tree_t *tr, tnode_t *tn, obj_id_t *arg, int pre_i)
{
    tnode_child_t *cs;
    int child_num;

    child_num = 0;
    for (int argi = 0; argi < tr->arg_size; ++argi){
        cs = tn->child + argi;
        child_num += cs->child_size;
        for (int i = 0; i < cs->child_size; ++i){
            //ASSERT(arg[argi] == UNDEF);
            if (arg[argi] == UNDEF){
                arg[argi] = cs->child[i]->obj_id;
                cs->child[i]->pre_mask |= tn->pre_mask;
                ++cs->child[i]->pre_unified;
                propagatePre(tr, cs->child[i], arg, pre_i);
                arg[argi] = UNDEF;
            }else{
                cs->child[i]->pre_mask |= tn->pre_mask;
                ++cs->child[i]->pre_unified;
                propagatePre(tr, cs->child[i], arg, pre_i);
            }
        }
    }

    // If all preconditions are unified and we are at leaf node we can
    // ground the action using assigned arguments.
    if (tn->pre_mask == tr->pre_mask && child_num == 0){
        // TODO: child_num == 0 should not be required
        // TODO: Check arg against action
        // TODO: Ground add effects and add them to a set of reachable
        //       facts
        // TODO: If grounding fails then it means that this argument
        //       assignement cannot be grounded -- can we utilize this
        //       somehow?
        //       Also, the reason of the failure cannot be types of
        //       arguments, or equality of arguments, because these things
        //       are checked at the beggining. Therefore the only reason
        //       can be negative preconditions on static predicates.
        // TODO: If grounding is successful, we can probably safe some
        //       memory removing part of tree. The question is whether is
        //       it useful.
        groundActionAddEff(tr->g, tr->action, arg);
    }
}

static void unifyPre(tree_t *tr, tnode_t *tn, obj_id_t *arg, int pre_i)
{
    tnode_child_t *cs;
    int child_num;

    ASSERT(!(tn->pre_mask & (1u << ((pre_mask_t)pre_i))));

    // TODO: Check action for equality and predicates?
    PRE_MASK_SET(tn, pre_i);
    ++tn->pre_unified;
    tn->flags.pre_unified = 1;
    propagatePre(tr, tn, arg, pre_i);
    return;

    child_num = 0;
    for (int argi = 0; argi < tr->arg_size; ++argi){
        cs = tn->child + argi;
        child_num += cs->child_size;
        for (int i = 0; i < cs->child_size; ++i){
            //ASSERT(arg[argi] == UNDEF);
            if (arg[argi] == UNDEF){
                arg[argi] = cs->child[i]->obj_id;
                unifyPre(tr, cs->child[i], arg, pre_i);
                arg[argi] = UNDEF;
            }else{
                unifyPre(tr, cs->child[i], arg, pre_i);
            }
        }
    }

    // If all preconditions are unified and we are at leaf node we can
    // ground the action using assigned arguments.
    //if (tn->pre_mask == tr->pre_mask && child_num == 0){
    if (tn->pre_mask == tr->pre_mask){
        ASSERT(tn->pre_unified == tr->pre_size);
        // TODO: child_num == 0 should not be required
        ASSERT(child_num == 0);
        // TODO: Check arg against action
        // TODO: Ground add effects and add them to a set of reachable
        //       facts
        // TODO: If grounding fails then it means that this argument
        //       assignement cannot be grounded -- can we utilize this
        //       somehow?
        //       Also, the reason of the failure cannot be types of
        //       arguments, or equality of arguments, because these things
        //       are checked at the beggining. Therefore the only reason
        //       can be negative preconditions on static predicates.
        // TODO: If grounding is successful, we can probably safe some
        //       memory removing part of tree. The question is whether is
        //       it useful.
        groundActionAddEff(tr->g, tr->action, arg);
    }
}

static tnode_t *unifyNew(tree_t *tr, tnode_t *tn, obj_id_t *arg,
                         int remain, const obj_id_t *arg_pre, int pre_i,
                         int static_fact);
static tnode_t *unifyNewArg(tree_t *tr, tnode_t *tn, obj_id_t *arg, int argi,
                            int remain, const obj_id_t *arg_pre, int pre_i,
                            int static_fact)
{
    tnode_t *new;

    arg[argi] = arg_pre[argi];
    new = tnodeAddChild(tr, tn, argi, arg[argi]);
    new->flags.static_arg = static_fact;
    if (remain - 1 > 0){
        unifyNew(tr, new, arg, remain - 1, arg_pre, pre_i, static_fact);
    }else{
        unifyPre(tr, new, arg, pre_i);
    }
    arg[argi] = UNDEF;
    return new;
}

static tnode_t *unifyNew(tree_t *tr, tnode_t *tn, obj_id_t *arg,
                         int remain, const obj_id_t *arg_pre, int pre_i,
                         int static_fact)
{
    tnode_t *new = NULL;

    // To eliminate at least some duplicates, first try to create a new
    // node using an argument that has some assignements on this level.
    for (int i = 0; i < tr->arg_size; ++i){
        tnode_child_t *cs = tn->child + i;
        if (cs->child_size > 0 && arg[i] == UNDEF && arg_pre[i] != UNDEF){
            return unifyNewArg(tr, tn, arg, i, remain, arg_pre, pre_i,
                               static_fact);
        }
    }

    for (int i = 0; i < tr->arg_size; ++i){
        if (arg[i] == UNDEF && arg_pre[i] != UNDEF){
            return unifyNewArg(tr, tn, arg, i, remain, arg_pre, pre_i,
                               static_fact);
        }
    }

    ASSERT(new != NULL);
    return new;
}

static void unify(tree_t *tr, tnode_t *tn,
                  obj_id_t *arg, int remain,
                  const obj_id_t *pre_arg, int pre_i,
                  int allow_new, int static_fact);

static int unifyArg(tree_t *tr, tnode_t *tn,
                    int argi, obj_id_t *arg, int remain,
                    const obj_id_t *arg_pre, int pre_i,
                    int allow_new, int static_fact)
{
    tnode_child_t *tnc = tn->child + argi;
    tnode_t *ch;
    int match = 0;

    ASSERT(tn->pre_unified == __builtin_popcount(tn->pre_mask));
    arg[argi] = arg_pre[argi];

    tnc = tn->child + argi;
    for (int i = 0; i < tnc->child_size; ++i){
        ch = tnc->child[i];
        ASSERT(ch->obj_id != UNDEF);

        if (ch->obj_id == arg[argi]){
            ASSERT(!(ch->pre_mask & (1u << pre_i)));
            ch->flags.static_arg = static_fact;
            // Found exact match on the argument
            unify(tr, ch, arg, remain - 1, arg_pre, pre_i, 1, static_fact);
            match = 1;

        }else if (arg[argi] == UNDEF){
            ASSERT(!(ch->pre_mask & (1u << pre_i)));
            // Argument is not set therefore we need to unify with all set
            // arguments
            arg[argi] = ch->obj_id;
            unify(tr, ch, arg, remain, arg_pre, pre_i, 0, static_fact);
            arg[argi] = UNDEF;
        }
    }

    arg[argi] = UNDEF;
    return match;
}

static void unify(tree_t *tr, tnode_t *tn, obj_id_t *arg, int remain,
                  const obj_id_t *arg_pre, int pre_i,
                  int allow_new, int static_fact)
{
    tnode_child_t *tnc;
    int match = 0;

    if (remain == 0){
        unifyPre(tr, tn, arg, pre_i);
        return;
        // TODO
    }

    for (int argi = 0; argi < tr->arg_size; ++argi){
        tnc = tn->child + argi;
        if (tnc->child_size > 0)
            match |= unifyArg(tr, tn, argi, arg, remain, arg_pre, pre_i,
                              allow_new, static_fact);
    }

    if (!match && !tn->flags.blocked && (allow_new || tn->flags.pre_unified)){
        unifyNew(tr, tn, arg, remain, arg_pre, pre_i, static_fact);
    }
    // TODO:
    /*
    if (!match && allow_new && !full){
        unifyNew(tr, tn, arg, remain, arg_pre, pre_i, static_fact);
    }else if (!match && !full){
        unifyNew(tr, tn, arg, remain, arg_pre, pre_i, static_fact);
    }
    */
}

static void treeUnify(tree_t *tr, const pddl_fact_t *fact, int pre_i,
                      int static_fact)
{
    const pddl_cond_atom_t *atom;
    obj_id_t arg[tr->arg_size], arg_pre[tr->arg_size];
    int num_args_set = 0;
    int param;

    /*
    fprintf(stderr, "Fact: ");
    pddlFactPrint(tr->g->pddl, fact, stderr);
    fprintf(stderr, " --> %s, pre_i: %d, arg_size: %d, pre_size: %d\n",
            tr->action->action->name, pre_i, tr->arg_size, tr->pre_size);
    fprintf(stderr, "Param sizes:");
    for (int i = 0; i < tr->arg_size; ++i){
        const int *obj;
        int size;
        obj = pddlTypesObjsByType(tr->action->type,
                                  tr->action->param_type[i], &size);
        fprintf(stderr, " %d:%d[", i, size);
        for (int j = 0; j < size; ++j)
            fprintf(stderr, "%d;", obj[j]);
        fprintf(stderr, "]");
    }
    fprintf(stderr, "\n");
    */
    //treePrint(tr, stderr);
    // TODO: check fact agains action
    // TODO: Static facts -- after using all of them disallow -1 on
    //       arguments of static facts.
    // TODO: Remove -1 nodes if all possible objects are already present
    //       and lock the argument
    if (!pddlPrepActionCheckFact(tr->action, pre_i, fact))
        return;

    atom = PDDL_COND_CAST(tr->action->pre.cond[pre_i], atom);
    for (int i = 0; i < tr->arg_size; ++i)
        arg_pre[i] = arg[i] = UNDEF;
    for (int i = 0; i < atom->arg_size; ++i){
        param = atom->arg[i].param;
        if (param >= 0)
            arg_pre[param] = fact->arg[i];
    }
    for (int i = 0; i < tr->arg_size; ++i){
        if (arg_pre[i] != UNDEF)
            ++num_args_set;
    }

    /*
    fprintf(stderr, "Fact: ");
    pddlFactPrint(tr->g->pddl, fact, stderr);
    for (int i = 0; i < tr->arg_size; ++i)
        fprintf(stderr, " %d", arg_pre[i]);
    fprintf(stderr, " | pre_i: %d, remain: %d\n", pre_i, num_args_set);
    */
    unify(tr, tr->root, arg, num_args_set, arg_pre, pre_i, 1, static_fact);
    //treePrint(tr, stderr);
}

static void _treeFixStatic(tree_t *tr, tnode_t *tn)
{
    tnode_child_t *cs;
    tnode_t *ch;
    int num_static;

    for (int argi = 0; argi < tr->arg_size; ++argi){
        cs = tn->child + argi;

        // If at least one child corresponds to a static argument, keep
        // only static children
        num_static = 0;
        for (int i = 0; i < cs->child_size; ++i)
            num_static += cs->child[i]->flags.static_arg;

        if (num_static > 0){
            for (int i = 0; i < cs->child_size; ++i){
                ch = cs->child[i];
                if (!ch->flags.static_arg){
                    tnodeDel(tr, ch);
                    cs->child[i] = NULL;
                }
            }
            int ins = 0;
            for (int i = 0; i < cs->child_size; ++i){
                if (cs->child[i] != NULL)
                    cs->child[ins++] = cs->child[i];
            }
            cs->child_size = ins;
        }

        for (int i = 0; i < cs->child_size; ++i){
            ch = cs->child[i];
            _treeFixStatic(tr, ch);
            tn->flags.blocked = 1;
        }
    }
}

static int _treeRemoveIncompleteStatic(tree_t *tr, tnode_t *tn)
{
    tnode_child_t *cs;
    int num_child = 0;

    for (int argi = 0; argi < tr->arg_size; ++argi){
        cs = tn->child + argi;
        for (int i = 0; i < cs->child_size; ++i){
            if (_treeRemoveIncompleteStatic(tr, cs->child[i])){
                tnodeDel(tr, cs->child[i]);
                cs->child[i] = NULL;
            }
        }
        int ins = 0;
        for (int i = 0; i < cs->child_size; ++i){
            if (cs->child[i] != NULL)
                cs->child[ins++] = cs->child[i];
        }
        cs->child_size = ins;
        num_child += cs->child_size;
    }

    if (num_child == 0
            && tn->pre_mask != tr->pre_static_mask //TODO --> .pre_unified
            && tn->flags.static_arg){
        return 1;
    }
    return 0;
}

static void treeFixStatic(tree_t *tr)
{
    // TODO: check the action agains the whole arg assignement at leafs
    _treeFixStatic(tr, tr->root);
    _treeRemoveIncompleteStatic(tr, tr->root);
}

static int treeInstantiateSmallArgs(tree_t *tr, tnode_t *tn, int arg_start,
                                    int arg_size, int arg_size_max)
{
    tnode_t *ch;
    const int *obj;
    int size;

    for (int argi = arg_start; argi < tr->arg_size; ++argi){
        if (tr->arg_max_size[argi] != arg_size)
            continue;
        obj = pddlTypesObjsByType(tr->action->type,
                                  tr->action->param_type[argi], &size);
        for (int i = 0; i < size; ++i){
            ch = tnodeAddChild(tr, tn, argi, obj[i]);
            treeInstantiateSmallArgs(tr, ch, argi + 1, arg_size, arg_size_max);
        }
        if (size > 0){
            tn->flags.blocked = 1;
            return 1;
        }
        return 0;
    }

    if (arg_size < arg_size_max)
        return treeInstantiateSmallArgs(tr, tn, 0, arg_size + 1, arg_size_max);
    tn->flags.pre_unified = 1;

    return 0;
}

static void predToPreAdd(pred_to_pre_t *p, int pre_id)
{
    ++p->size;
    p->pre = BOR_REALLOC_ARR(p->pre, int, p->size);
    p->pre[p->size - 1] = pre_id;
}

static void treeInit(tree_t *tr, ground_t *g, int action_id)
{
    pddl_prep_action_t *a = g->action.action + action_id;
    const pddl_cond_atom_t *atom;
    const pddl_pred_t *pred;

    // TODO: Check limits on obj_id_t, pre_mask_t, ...

    bzero(tr, sizeof(*tr));
    tr->g = g;
    tr->action_id = action_id;
    tr->action = a;
    tr->arg_size = a->param_size;
    tr->pre_size = a->pre.size;
    tr->root = tnodeNew(tr, NULL, UNDEF);

    tr->arg_max_size = BOR_ALLOC_ARR(int, tr->arg_size);
    for (int i = 0; i < tr->arg_size; ++i)
        pddlTypesObjsByType(a->type, a->param_type[i], tr->arg_max_size + i);

    for (int i = 0; i < tr->pre_size; ++i){
        tr->pre_mask = (tr->pre_mask << 1u) | 1u;
        atom = PDDL_COND_CAST(tr->action->pre.cond[i], atom);
        pred = tr->g->pddl->pred.pred + atom->pred;
        if (pddlPredIsStatic(pred))
            tr->pre_static_mask |= (1u << i);
    }

    tr->pred_to_pre = BOR_CALLOC_ARR(pred_to_pre_t, g->pddl->pred.size);
    for (int i = 0; i < a->pre.size; ++i){
        atom = PDDL_COND_CAST(a->pre.cond[i], atom);
        predToPreAdd(tr->pred_to_pre + atom->pred, i);
    }

    // TODO: move constans 1 and 3 into either parameter of grounding or
    //       define constants. Consider also instantiation also a small
    //       number (1 or 2) of bigger arguments.
    treeInstantiateSmallArgs(tr, tr->root, 0, 1, 3);
    //treePrint(tr, stderr);
}

static void treeFree(tree_t *tr)
{
    for (int i = 0; i < tr->g->pddl->pred.size; ++i){
        if (tr->pred_to_pre[i].pre != NULL)
            BOR_FREE(tr->pred_to_pre[i].pre);
    }
    if (tr->pred_to_pre != NULL)
        BOR_FREE(tr->pred_to_pre);
    tnodeDel(tr, tr->root);
    if (tr->arg_max_size != NULL)
        BOR_FREE(tr->arg_max_size);
}

static void tnodePrint(tree_t *tr, tnode_t *tn, int argi, int offset, FILE *fout)
{
    int off = 0, p = 0;

    off += fprintf(fout, "%d", argi);
    if (tn->obj_id == UNDEF){
        off += fprintf(fout, ":X");
    }else{
        off += fprintf(fout, ":%d", tn->obj_id);
    }
    off += fprintf(fout, ":P%x", tn->pre_mask);
    off += fprintf(fout, ":%d", tn->pre_unified);
    if (tn->flags.blocked)
        off += fprintf(fout, ":B");
    if (tn->flags.pre_unified)
        off += fprintf(fout, ":U");
    if (tn->flags.static_arg)
        off += fprintf(fout, ":S");
    if (tn->pre_mask == tr->pre_mask)
        off += fprintf(fout, "*");

    for (int argi = 0; argi < tr->arg_size; ++argi){
        tnode_child_t *cs = tn->child + argi;
        for (int i = 0; i < cs->child_size; ++i){
            if (p){
                fprintf(fout, "\n");
                for (int i = 0; i < offset + off; ++i)
                    fprintf(fout, " ");
                fprintf(fout, "`");
            }else{
                fprintf(fout, " ");
                p = 1;
            }
            tnodePrint(tr, cs->child[i], argi, offset + off + 1, fout);
        }
    }

    if (offset == 0)
        fprintf(fout, "\n");
}

static void treePrint(tree_t *tr, FILE *fout)
{
    fprintf(fout, "Tree for %s, arg_size: %d, pre_size: %d, pre_mask: %x"
                  " root-blocked: %d, param-size:",
            tr->action->action->name, tr->arg_size, tr->pre_size,
            tr->pre_mask, tr->root->flags.blocked);
    for (int i = 0; i < tr->arg_size; ++i)
        fprintf(fout, " %d:%d", i, tr->arg_max_size[i]);
    fprintf(fout, "\n");
    for (int argi = 0; argi < tr->arg_size; ++argi){
        tnode_child_t *cs = tr->root->child + argi;
        for (int i = 0; i < cs->child_size; ++i)
            tnodePrint(tr, cs->child[i], argi, 0, fout);
    }
}


static void groundInitFact(ground_t *g, const pddl_t *pddl)
{
    const pddl_fact_t *fact;

    for (int i = 0; i < pddl->init_fact.fact_size; ++i){
        fact = pddl->init_fact.fact[i];
        if (pddlFactIsStatic(pddl, fact)){
            pddlFactsAdd(&g->static_fact, fact);
        }else{
            pddlFactsAdd(&g->fact, fact);
        }
    }
}

static void groundInit(ground_t *g, const pddl_t *pddl)
{
    bzero(g, sizeof(*g));
    g->pddl = pddl;
    pddlPrepActionsInit(pddl, &g->action);
    pddlFactsInit(&g->static_fact);
    pddlFactsInit(&g->fact);
    pddlStripsOpsInit(&g->op);

    groundInitFact(g, pddl);

    g->tree = BOR_ALLOC_ARR(tree_t, g->action.size);
    for (int i = 0; i < g->action.size; ++i)
        treeInit(g->tree + i, g, i);
}

static void groundFree(ground_t *g)
{
    for (int i = 0; i < g->action.size; ++i)
        treeFree(g->tree + i);
    if (g->tree != NULL)
        BOR_FREE(g->tree);
    pddlStripsOpsFree(&g->op);
    pddlFactsFree(&g->fact);
    pddlFactsFree(&g->static_fact);
    pddlPrepActionsFree(&g->action);
    groundArgsFree(&g->ground_args);
}

static void printAction(ground_t *g, const pddl_prep_action_t *a, int *arg)
{
    fprintf(stderr, "%s", a->action->name);
    for (int i = 0; i < a->param_size; ++i){
        if (arg[i] >= 0){
            fprintf(stderr, " %s", g->pddl->obj.obj[arg[i]].name);
        }else{
            fprintf(stderr, " %s", a->action->param.param[i].name);
        }
    }
    fprintf(stderr, "\n");
}

static void _groundActionAddEff(ground_t *g,
                                const pddl_prep_action_t *a,
                                int *arg, int argi)
{
    const int *obj;
    int size;

    // Skip bound arguments
    for (; argi < a->param_size && arg[argi] >= 0; ++argi);
    if (argi < a->param_size){
        obj = pddlTypesObjsByType(a->type, a->param_type[argi], &size);
        for (int i = 0; i < size; ++i){
            arg[argi] = obj[i];
            _groundActionAddEff(g, a, arg, argi + 1);
            arg[argi] = -1;
        }
        return;
    }

    if (!pddlPrepActionCheck(a, &g->static_fact, arg)){
        //pddlActionPrint(g->pddl, a->action, stderr);
        fprintf(stderr, "FAIL: ");
        printAction(g, a, arg);
        return;
    }

    fprintf(stderr, "SUCCESS: ");
    printAction(g, a, arg);

    const pddl_cond_atom_t *atom;
    PDDL_FACT_FOR_GROUND2(fact, a->max_arg_size);
    for (int i = 0; i < a->add_eff.size; ++i){
        atom = PDDL_COND_CAST(a->add_eff.cond[i], atom);
        pddlCondAtomGroundFact(atom, arg, &fact);
        pddlFactsAdd(&g->fact, &fact);

        /*
        fprintf(stderr, "ADD FACT ");
        pddlFactPrint(g->pddl, &fact, stderr);
        fprintf(stderr, "\n");
        */
    }

    groundArgsAdd(&g->ground_args, a - g->action.action, a, arg);
}

static void groundActionAddEff(ground_t *g,
                               const pddl_prep_action_t *a,
                               const obj_id_t *oarg)
{
    int arg[a->param_size];

    // TODO: What about zero params actions
    for (int i = 0; i < a->param_size; ++i)
        arg[i] = (oarg[i] == UNDEF ? -1 : oarg[i]);
    _groundActionAddEff(g, a, arg, 0);
}

static char *groundOpName(const pddl_t *pddl,
                          const pddl_action_t *action,
                          const int *args)
{
    int i, slen;
    char *name, *cur;

    slen = strlen(action->name) + 2 + 1;
    for (i = 0; i < action->param.size; ++i)
        slen += 1 + strlen(pddl->obj.obj[args[i]].name);

    cur = name = BOR_ALLOC_ARR(char, slen);
    cur += sprintf(cur, "(%s", action->name);
    for (i = 0; i < action->param.size; ++i)
        cur += sprintf(cur, " %s", pddl->obj.obj[args[i]].name);
    cur += sprintf(cur, ")");

    return name;
}

static int groundAssign(int atom_max_arg_size,
                        const int *arg,
                        const pddl_cond_arr_t *atoms,
                        const pddl_facts_t *funcs)
{
    const pddl_cond_assign_t *atom;
    PDDL_FACT_FOR_GROUND2(func, atom_max_arg_size);
    const pddl_fact_t *fvalue;
    int func_id;
    int cost = 0;

    for (int i = 0; i < atoms->size; ++i){
        atom = PDDL_COND_CAST(atoms->cond[i], assign);
        if (atom->fvalue != NULL){
            pddlCondAtomGroundFact(atom->fvalue, arg, &func);
            func_id = pddlFactsFind(funcs, &func);
            if (func_id >= 0){
                fvalue = funcs->fact[func_id];
                cost += fvalue->func_val;
            }else{
                // TODO
                fprintf(stderr, "ERROR: Invalid function -- cannot ground!\n");
                exit(-1);
            }
        }else{
            cost += atom->value;
        }
    }

    return cost;
}

static void groundAtoms(int atom_max_arg_size,
                        const int *arg,
                        const pddl_cond_arr_t *atoms,
                        const pddl_facts_t *facts,
                        pddl_fact_id_arr_t *out)
{
    const pddl_cond_atom_t *atom;
    PDDL_FACT_FOR_GROUND2(fact, atom_max_arg_size);
    int fact_id;

    for (int i = 0; i < atoms->size; ++i){
        atom = PDDL_COND_CAST(atoms->cond[i], atom);
        pddlCondAtomGroundFact(atom, arg, &fact);
        fact_id = pddlFactsFind(facts, &fact);
        if (fact_id >= 0){
            pddlFactIdArrAdd(out, fact_id);
        }
    }
}

static void groundActions(ground_t *g)
{
    ground_args_t *ga, *parent_ga;
    const pddl_prep_action_t *a;
    pddl_strips_op_t op;
    pddl_strips_op_t *parent;
    char *name;
    int op_id;

    groundArgsSortAndUniq(&g->ground_args);
    for (int i = 0; i < g->ground_args.size; ++i){
        const ground_args_t *ga = g->ground_args.arg + i;
        fprintf(stderr, "G[%d] id: %d, parent: %d |", i,
                ga->action_id, ga->action->parent_action);
        for (int j = 0; j < ga->action->param_size; ++j)
            fprintf(stderr, " %d", ga->arg[j]);
        fprintf(stderr, "\n");
    }

    parent_ga = NULL;
    for (int i = 0; i < g->ground_args.size; ++i){
        ga = g->ground_args.arg + i;
        a = ga->action;
        ASSERT(pddlPrepActionCheck(a, &g->static_fact, ga->arg));

        pddlStripsOpInit(&op);
        // Ground precontions, add and delete effects and set cost
        groundAtoms(a->max_arg_size, ga->arg, &a->pre, &g->fact, &op.pre);
        groundAtoms(a->max_arg_size, ga->arg, &a->add_eff, &g->fact,
                    &op.add_eff);
        groundAtoms(a->max_arg_size, ga->arg, &a->del_eff, &g->fact,
                    &op.del_eff);
        op.cost = 1;
        if (g->pddl->metric){
            op.cost = groundAssign(a->max_arg_size, ga->arg, &a->assign,
                                   &g->pddl->init_func);
        }
        name = groundOpName(g->pddl, a->action, ga->arg);

        // Make the operator well-formed
        pddlStripsOpFinalize(&op, name);

        // Use only operators with effects
        if (op.add_eff.size == 0 && op.del_eff.size == 0){
            pddlStripsOpFree(&op);
            continue;
        }

        if (a->parent_action >= 0){
            // If the operator corresponds to a conditional effect the
            // parent must be known already, because this is the way we
            // sorted ground_args_t structures.
            ASSERT(parent_ga != NULL);
            parent = g->op.op[parent_ga->op_id];

            // Find out preconditions that belong only to the conditional
            // effect.
            pddlFactIdArrMinus(&op.pre, &parent->pre);
            if (op.pre.size > 0){
                // Create conditional effect if necessary
                pddlStripsOpAddCondEff(parent, &op);

            }else{
                // If precondition of the conditional effect is empty, then
                // we can merge conditional effect directly to the parent
                // operator.
                // The operators are hashed only using its name so we can
                // merge effects with re-inserting operator.
                pddlStripsOpAddEffFromOp(parent, &op);

                // If operator was well-formed before it must remain
                // well-formed.
                ASSERT(parent->add_eff.size > 0 || parent->del_eff.size > 0);
            }

        }else{
            op_id = pddlStripsOpsAdd(&g->op, &op);
            ga->op_id = op_id;
            parent_ga = ga;
        }

        pddlStripsOpFree(&op);
    }

    fprintf(stderr, "Ops[%d]:\n", g->op.op_size);
    pddlStripsOpsPrint(&g->op, stderr);
}


static void groundStaticFacts(ground_t *g)
{
    for (int i = 0; i < g->static_fact.fact_size; ++i){
        const pddl_fact_t *fact = g->static_fact.fact[i];
        /*
        fprintf(stderr, "Pop Fact: ");
        pddlFactPrint(g.pddl, fact, stderr);
        fprintf(stderr, " \n");
        */

        for (int j = 0; j < g->action.size; ++j){
            tree_t *tr = g->tree + j;
            for (int k = 0; k < tr->pred_to_pre[fact->pred].size; ++k){
                treeUnify(tr, fact, tr->pred_to_pre[fact->pred].pre[k], 1);
            }
        }
    }

    fprintf(stderr, "STATIC END\n");
    for (int i = 0; i < g->action.size; ++i){
        treeFixStatic(g->tree + i);
        //treePrint(g->tree + i, stderr);
    }
}

static void groundFacts(ground_t *g)
{
    for (int i = 0; i < g->fact.fact_size; ++i){
        const pddl_fact_t *fact = g->fact.fact[i];
        /*
        fprintf(stderr, "Pop Fact: ");
        pddlFactPrint(g.pddl, fact, stderr);
        fprintf(stderr, " \n");
        */

        for (int j = 0; j < g->action.size; ++j){
            tree_t *tr = g->tree + j;
            for (int k = 0; k < tr->pred_to_pre[fact->pred].size; ++k){
                treeUnify(tr, fact, tr->pred_to_pre[fact->pred].pre[k], 1);
            }
        }
    }
}

void __pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl)
{
    ground_t g;

    groundInit(&g, pddl);
    groundStaticFacts(&g);
    groundFacts(&g);
    groundActions(&g);

#if 0
    for (int i = 0; i < g.fact.fact_size; ++i){
        const pddl_fact_t *fact = g.fact.fact[i];
        /*
        fprintf(stderr, "Pop Fact: ");
        pddlFactPrint(g.pddl, fact, stderr);
        fprintf(stderr, " \n");
        */

        for (int j = 0; j < g.action.size; ++j){
            tree_t *tr = g.tree + j;
            for (int k = 0; k < tr->pred_to_pre[fact->pred].size; ++k){
                treeUnify(tr, fact, tr->pred_to_pre[fact->pred].pre[k]);
            }
        }
    }

    fprintf(stderr, "END:\n");
    for (int j = 0; j < g.action.size; ++j){
        tree_t *tr = g.tree + j;
        //treePrint(tr, stderr);
    }
#endif

    groundFree(&g);
}
