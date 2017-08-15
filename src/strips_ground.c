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
#include "pddl/err.h"
#include "err.h"
#include "assert.h"


// TODO: Move this to common.h and define also pddl_type_id_t, etc...
typedef int obj_id_t;
#define UNDEF -1

#ifdef PDDL_DEBUG
typedef uint32_t pre_mask_t;
#endif /* PDDL_DEBUG */

struct ground;

struct tnode_flags {
    unsigned char blocked:1; /*!< True if no new children are allowed */
    unsigned char pre_unified:1; /*!< True if the node unified
                                      a new precondition */
    unsigned char static_arg:1; /*!< True if the node corresponds to an
                                     argument of a static fact */
} bor_packed;
typedef struct tnode_flags tnode_flags_t;

struct tnode {
    int argi; /*!< Index of the corresponding argument */
    obj_id_t obj_id; /*!< Assigned object ID */
    int pre_unified; /*!< Number of unified preconditions */
#ifdef PDDL_DEBUG
    pre_mask_t pre_mask; /*!< Bits set on positions where precondition is set */
#endif /* PDDL_DEBUG */
    tnode_flags_t flags;
    struct tnode **child;
    int child_size;
    int child_alloc;
} bor_packed;
typedef struct tnode tnode_t;

#define TNODE_FOR_EACH_CHILD(TN, CH) \
    for (int __i = 0; __i < (TN)->child_size \
            && ((CH) = (TN)->child[__i]); ++__i)


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
    int pre_static_size;
#ifdef PDDL_DEBUG
    pre_mask_t pre_mask;
    pre_mask_t pre_static_mask; /*!< Mask only on static preconditions */
#endif /* PDDL_DEBUG */
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
    pddl_strips_t *strips;
    const pddl_t *pddl;
    pddl_prep_actions_t action;
    pddl_facts_t static_fact;
    tree_t *tree;
    ground_args_arr_t ground_args;
};
typedef struct ground ground_t;

#ifdef PDDL_DEBUG
static void treePrint(tree_t *tr, FILE *fout);
#endif /* PDDL_DEBUG */

static tnode_t *tnodeNew(tree_t *t, tnode_t *parent,
                         int argi, obj_id_t obj_id);
static void tnodeDel(tree_t *tr, tnode_t *t);
static tnode_t *tnodeAddChild(tree_t *t, tnode_t *par,
                              int argi, obj_id_t obj_id);

static void treeInit(tree_t *tr, ground_t *g, int action_id);
static void treeFree(tree_t *tr);

static void groundArgsFree(ground_args_arr_t *ga);
static void groundArgsAdd(ground_args_arr_t *ga, int action_id,
                          const pddl_prep_action_t *action,
                          const int *arg);
static void groundArgsSortAndUniq(ground_args_arr_t *ga);

static int unifyStaticFacts(ground_t *g);
static int unifyFacts(ground_t *g);

static int groundActions(ground_t *g);
static void groundActionAddEff(ground_t *g,
                               const pddl_prep_action_t *a,
                               const obj_id_t *oarg);
static void groundActionAddEffEmptyPre(ground_t *g,
                                       const pddl_prep_action_t *a);
static char *groundOpName(const pddl_t *pddl,
                          const pddl_action_t *action,
                          const int *args);



/*** tnode_t ***/
static tnode_t *tnodeNew(tree_t *t, tnode_t *parent,
                         int argi, obj_id_t obj_id)
{
    tnode_t *n;

    n = BOR_ALLOC(tnode_t);
    bzero(n, sizeof(*n));
    n->argi = argi;
    n->obj_id = obj_id;
    if (parent != NULL){
#ifdef PDDL_DEBUG
        n->pre_mask = parent->pre_mask;
#endif /* PDDL_DEBUG */
        n->pre_unified = parent->pre_unified;
    }
    return n;
}

static void tnodeDel(tree_t *tr, tnode_t *t)
{
    tnode_t *ch;
    TNODE_FOR_EACH_CHILD(t, ch)
        tnodeDel(tr, ch);
    if (t->child != NULL)
        BOR_FREE(t->child);
    BOR_FREE(t);
}

static void tnodeReserveChild(tree_t *tr, tnode_t *n)
{
    if (n->child_size == n->child_alloc){
        if (n->child_alloc == 0)
            n->child_alloc = 1;
        n->child_alloc *= 2;
        n->child = BOR_REALLOC_ARR(n->child, tnode_t *, n->child_alloc);
    }
}

static tnode_t *tnodeAddChild(tree_t *t, tnode_t *par,
                              int argi, obj_id_t obj_id)
{
    tnode_t *n = tnodeNew(t, par, argi, obj_id);
    tnodeReserveChild(t, par);
    par->child[par->child_size++] = n;
    return n;
}
/*** tnode_t END ***/


/*** tree_t ***/
static void predToPreAdd(pred_to_pre_t *p, int pre_id)
{
    ++p->size;
    p->pre = BOR_REALLOC_ARR(p->pre, int, p->size);
    p->pre[p->size - 1] = pre_id;
}

static int instantiateArgs(tree_t *tr, tnode_t *tn, int arg_start,
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
            instantiateArgs(tr, ch, argi + 1, arg_size, arg_size_max);
        }
        if (size > 0){
            tn->flags.blocked = 1;
            return 1;
        }
        return 0;
    }

    if (arg_size < arg_size_max)
        return instantiateArgs(tr, tn, 0, arg_size + 1, arg_size_max);
    tn->flags.pre_unified = 1;

    return 0;
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
    tr->pre_static_size = 0;
    tr->root = tnodeNew(tr, NULL, -1, UNDEF);

    tr->arg_max_size = BOR_ALLOC_ARR(int, tr->arg_size);
    for (int i = 0; i < tr->arg_size; ++i)
        pddlTypesObjsByType(a->type, a->param_type[i], tr->arg_max_size + i);

    for (int i = 0; i < tr->pre_size; ++i){
#ifdef PDDL_DEBUG
        tr->pre_mask = (tr->pre_mask << 1u) | 1u;
#endif /* PDDL_DEBUG */
        atom = PDDL_COND_CAST(tr->action->pre.cond[i], atom);
        pred = tr->g->pddl->pred.pred + atom->pred;
        if (pddlPredIsStatic(pred)){
            ++tr->pre_static_size;
#ifdef PDDL_DEBUG
            tr->pre_static_mask |= (1u << i);
#endif /* PDDL_DEBUG */
        }
    }

    tr->pred_to_pre = BOR_CALLOC_ARR(pred_to_pre_t, g->pddl->pred.size);
    for (int i = 0; i < a->pre.size; ++i){
        atom = PDDL_COND_CAST(a->pre.cond[i], atom);
        predToPreAdd(tr->pred_to_pre + atom->pred, i);
    }

    // TODO: move constans 1 and 3 into either parameter of grounding or
    //       define constants. Consider also instantiation also a small
    //       number (1 or 2) of bigger arguments.
    instantiateArgs(tr, tr->root, 0, 1, 3);
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
/*** tree_t END ***/


/*** ground_args_t ***/
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
        if (g1->action->parent_action < 0)
            return -1;
        if (g2->action->parent_action < 0)
            return 1;
        return g1->action_id - g2->action_id;
    }
    return g1_action_id - g2_action_id;
}

static void groundArgsSortAndUniq(ground_args_arr_t *ga)
{
    int ins;

    if (ga->arg == 0)
        return;

    borSort(ga->arg, ga->size, sizeof(ground_args_t), groundArgsCmp, NULL);

    // Remove duplicates
    // TODO: Is it even possible that there are duplicates?
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
/*** ground_args_t END ***/


/*** unify ***/
static void propagatePre(tree_t *tr, tnode_t *tn, obj_id_t *arg)
{
    tnode_t *ch;

    // If all preconditions are unified, we can ground the action using
    // assigned arguments. Note that we don't actually need to be in a
    // leaf.
    if (tn->pre_unified == tr->pre_size){
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
        return;
    }

    TNODE_FOR_EACH_CHILD(tn, ch){
#ifdef PDDL_DEBUG
        ch->pre_mask |= tn->pre_mask;
#endif /* PDDL_DEBUG */
        ++ch->pre_unified;
        if (arg[ch->argi] == UNDEF){
            arg[ch->argi] = ch->obj_id;
            propagatePre(tr, ch, arg);
            arg[ch->argi] = UNDEF;
        }else{
            propagatePre(tr, ch, arg);
        }
    }
}

static void unifyPre(tree_t *tr, tnode_t *tn, obj_id_t *arg, int pre_i)
{
    // TODO: Check action for equality and predicates?
#ifdef PDDL_DEBUG
    ASSERT(!(tn->pre_mask & (1u << ((pre_mask_t)pre_i))));
    tn->pre_mask |= ((pre_mask_t)1u << ((pre_mask_t)pre_i));
#endif /* PDDL_DEBUG */
    ++tn->pre_unified;
    tn->flags.pre_unified = 1;
    propagatePre(tr, tn, arg);
}

static void unifyNew(tree_t *tr, tnode_t *tn, obj_id_t *arg,
                     int remain, const obj_id_t *arg_pre, int pre_i,
                     int static_fact);
static void unifyNewArg(tree_t *tr, tnode_t *tn, obj_id_t *arg, int argi,
                        int remain, const obj_id_t *arg_pre, int pre_i,
                        int static_fact)
{
    tnode_t *new;

    arg[argi] = arg_pre[argi];
    new = tnodeAddChild(tr, tn, argi, arg[argi]);
    if (static_fact)
        new->flags.static_arg = 1;
    if (remain - 1 > 0){
        unifyNew(tr, new, arg, remain - 1, arg_pre, pre_i, static_fact);
    }else{
        unifyPre(tr, new, arg, pre_i);
    }
    arg[argi] = UNDEF;
}

static void unifyNew(tree_t *tr, tnode_t *tn, obj_id_t *arg,
                     int remain, const obj_id_t *arg_pre, int pre_i,
                     int static_fact)
{
    tnode_t *ch;

    // To reduce branching, first try to create a new
    // node using an argument that has some assignements on this level.
    TNODE_FOR_EACH_CHILD(tn, ch){
        if (arg[ch->argi] == UNDEF && arg_pre[ch->argi] != UNDEF){
            unifyNewArg(tr, tn, arg, ch->argi, remain, arg_pre, pre_i,
                        static_fact);
            return;
        }
    }

    for (int i = 0; i < tr->arg_size; ++i){
        if (arg[i] == UNDEF && arg_pre[i] != UNDEF){
            unifyNewArg(tr, tn, arg, i, remain, arg_pre, pre_i, static_fact);
            return;
        }
    }
}

static void unify(tree_t *tr, tnode_t *tn, obj_id_t *arg, int remain,
                  const obj_id_t *arg_pre, int pre_i,
                  int parent_match, int static_fact)
{
    tnode_t *ch;
    int match = 0;

    if (remain == 0){
        unifyPre(tr, tn, arg, pre_i);
        return;
    }

    TNODE_FOR_EACH_CHILD(tn, ch){
        ASSERT(ch->obj_id != UNDEF);
        arg[ch->argi] = arg_pre[ch->argi];
        if (ch->obj_id == arg[ch->argi]){
            ASSERT(!(ch->pre_mask & (1u << pre_i)));
            if (static_fact)
                ch->flags.static_arg = 1;
            // Found exact match on the argument
            unify(tr, ch, arg, remain - 1, arg_pre, pre_i, 1, static_fact);
            match = 1;

        }else if (arg[ch->argi] == UNDEF){
            ASSERT(!(ch->pre_mask & (1u << pre_i)));
            // Argument is not set therefore we need to unify with all set
            // arguments
            arg[ch->argi] = ch->obj_id;
            unify(tr, ch, arg, remain, arg_pre, pre_i, 0, static_fact);
            arg[ch->argi] = UNDEF;
        }
        arg[ch->argi] = UNDEF;
    }

    // Create a new branch only if all of the following holds
    // 1) no argument could be matched in the current node
    // 2) the current node is allowed to have more children (it could be
    //    blocked due to static facts)
    // 3) there was match in the parent node
    //    or the current node corresponds to the end of some previously
    //    unified fact
    if (!match && !tn->flags.blocked && (parent_match || tn->flags.pre_unified))
        unifyNew(tr, tn, arg, remain, arg_pre, pre_i, static_fact);
}

static void unifyTree(tree_t *tr, const pddl_fact_t *fact, int pre_i,
                      int static_fact)
{
    const pddl_cond_atom_t *atom;
    obj_id_t arg[tr->arg_size], arg_pre[tr->arg_size];
    int num_args_set = 0;
    int param;

    // TODO: check fact agains action
    // TODO: Static facts -- after using all of them disallow -1 on
    //       arguments of static facts.
    // TODO: Remove -1 nodes if all possible objects are already present
    //       and lock the argument
    // Check whether the fact can be unified -- this test is not enough but
    // it can filter out some facts.
    if (!pddlPrepActionCheckFact(tr->action, pre_i, fact))
        return;

    // Initialize arg[] to undef -- this array will be filled with unified
    // arguments in unify() recursive call.
    for (int i = 0; i < tr->arg_size; ++i)
        arg_pre[i] = arg[i] = UNDEF;

    // Set arg_pre[] according to the fact's arguments and count the number
    // of set arguments.
    atom = PDDL_COND_CAST(tr->action->pre.cond[pre_i], atom);
    for (int i = 0; i < atom->arg_size; ++i){
        param = atom->arg[i].param;
        if (param >= 0 && arg_pre[param] == UNDEF){
            arg_pre[param] = fact->arg[i];
            ++num_args_set;
        }
    }

    // Recursivelly unify arguments
    unify(tr, tr->root, arg, num_args_set, arg_pre, pre_i, 1, static_fact);
}

static void _fixStatic(tree_t *tr, tnode_t *tn)
{
    tnode_t *ch;
    int prune[tr->arg_size];

    for (int i = 0; i < tr->arg_size; ++i)
        prune[i] = 0;
    TNODE_FOR_EACH_CHILD(tn, ch){
        if (ch->flags.static_arg)
            prune[ch->argi] = 1;
    }

    for (int i = 0; i < tn->child_size; ++i){
        ch = tn->child[i];
        ASSERT(ch != NULL);
        if (!prune[ch->argi])
            continue;
        if (!ch->flags.static_arg){
            tnodeDel(tr, ch);
            tn->child[i] = tn->child[--tn->child_size];
            --i;
        }
    }

    TNODE_FOR_EACH_CHILD(tn, ch)
        _fixStatic(tr, ch);

    if (tn->child_size > 0)
        tn->flags.blocked = 1;
}

static int removeIncompleteStatic(tree_t *tr, tnode_t *tn)
{
    tnode_t *ch;

    for (int i = 0; i < tn->child_size; ++i){
        ch = tn->child[i];
        if (removeIncompleteStatic(tr, ch)){
            tnodeDel(tr, ch);
            tn->child[i] = tn->child[--tn->child_size];
            --i;
        }
    }

    if (tn->child_size == 0
            && tn->pre_unified != tr->pre_static_size
            && tn->flags.static_arg){
        return 1;
    }
    return 0;
}

static void treeFixStatic(tree_t *tr)
{
    // TODO: check the action agains the whole arg assignement at leafs
    _fixStatic(tr, tr->root);
    removeIncompleteStatic(tr, tr->root);

    // If the action has any static preconditions, they must be already in
    // place therefore we can block the root node. This fixes the problem
    // with actions that cannot be grounded because there are no
    // corresponding static facts.
    if (tr->pre_static_size > 0)
        tr->root->flags.blocked = 1;
}

static void _unifyFacts(ground_t *g, pddl_facts_t *fs, int static_fact)
{
    for (int i = 0; i < fs->fact_size; ++i){
        const pddl_fact_t *fact = fs->fact[i];

        for (int j = 0; j < g->action.size; ++j){
            tree_t *tr = g->tree + j;
            for (int k = 0; k < tr->pred_to_pre[fact->pred].size; ++k){
                unifyTree(tr, fact, tr->pred_to_pre[fact->pred].pre[k],
                          static_fact);
            }
        }
    }
}

static int unifyStaticFacts(ground_t *g)
{
    // First ground actions without preconditions
    for (int i = 0; i < g->action.size; ++i){
        if (g->tree[i].pre_size == 0)
            groundActionAddEffEmptyPre(g, g->action.action + i);
    }

    _unifyFacts(g, &g->static_fact, 1);
    for (int i = 0; i < g->action.size; ++i)
        treeFixStatic(g->tree + i);

    return 0;
}

static int unifyFacts(ground_t *g)
{
    _unifyFacts(g, &g->strips->fact, 0);
    return 0;
}
/*** unify END ***/


static void _groundActionAddEff(ground_t *g,
                                const pddl_prep_action_t *a,
                                int *arg, int argi)
{
    const int *obj;
    int size;

    // Skip bound arguments
    for (; argi < a->param_size && arg[argi] >= 0; ++argi);
    // and try every possible object for the unbound arguments
    if (argi < a->param_size){
        obj = pddlTypesObjsByType(a->type, a->param_type[argi], &size);
        for (int i = 0; i < size; ++i){
            arg[argi] = obj[i];
            _groundActionAddEff(g, a, arg, argi + 1);
            arg[argi] = -1;
        }
        return;
    }

    if (!pddlPrepActionCheck(a, &g->static_fact, arg))
        return;

    const pddl_cond_atom_t *atom;
    PDDL_FACT_FOR_GROUND2(fact, a->max_arg_size);
    for (int i = 0; i < a->add_eff.size; ++i){
        atom = PDDL_COND_CAST(a->add_eff.cond[i], atom);
        if (pddlCondAtomGroundFact(atom, arg, &fact) != 0){
            FATAL("Cannot ground atom to a fact even though all parameters"
                  " are bound to objects. This is definitely a bug."
                  " The culprit action is %s.",
                  groundOpName(g->pddl, a->action, arg));
        }
        pddlFactsAdd(&g->strips->fact, &fact);
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

static void groundActionAddEffEmptyPre(ground_t *g,
                                       const pddl_prep_action_t *a)
{
    ASSERT(a->pre.size == 0);
    int arg[a->param_size];
    for (int i = 0; i < a->param_size; ++i)
        arg[i] = UNDEF;
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
    sprintf(cur, ")");

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

    // Only (increase (total-cost) ...) is allowed.
    for (int i = 0; i < atoms->size; ++i){
        atom = PDDL_COND_CAST(atoms->cond[i], assign);
        if (atom->fvalue != NULL){
            pddlCondAtomGroundFact(atom->fvalue, arg, &func);
            func_id = pddlFactsFind(funcs, &func);
            ASSERT_RUNTIME(func_id >= 0);
            fvalue = funcs->fact[func_id];
            cost += fvalue->func_val;
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
                        bor_iset_t *out)
{
    const pddl_cond_atom_t *atom;
    PDDL_FACT_FOR_GROUND2(fact, atom_max_arg_size);
    int fact_id;

    for (int i = 0; i < atoms->size; ++i){
        atom = PDDL_COND_CAST(atoms->cond[i], atom);
        pddlCondAtomGroundFact(atom, arg, &fact);
        fact_id = pddlFactsFind(facts, &fact);
        // Filter out static facts
        if (fact_id >= 0)
            borISetAdd(out, fact_id);
    }
}

static int setUpOp(ground_t *g, pddl_strips_op_t *op,
                    const ground_args_t *ga)
{
    const pddl_prep_action_t *a = ga->action;
    char *name;

    // Different operator cost for the conditional effects is not allowed
    if (a->parent_action >= 0 && a->assign.size > 0)
        ERR_RET2(-1, "Costs in conditional effects are not supported.");

    // Ground precontions, add and delete effects and set cost
    groundAtoms(a->max_arg_size, ga->arg, &a->pre,
                &g->strips->fact, &op->pre);
    groundAtoms(a->max_arg_size, ga->arg, &a->add_eff,
                &g->strips->fact, &op->add_eff);
    groundAtoms(a->max_arg_size, ga->arg, &a->del_eff,
                &g->strips->fact, &op->del_eff);
    op->cost = 1;
    if (g->pddl->metric){
        op->cost = groundAssign(a->max_arg_size, ga->arg, &a->assign,
                                &g->pddl->init_func);
    }
    name = groundOpName(g->pddl, a->action, ga->arg);

    // Make the operator well-formed
    pddlStripsOpFinalize(op, name);

    return 0;
}

static void groundCondEff(ground_t *g, pddl_strips_op_t *op,
                          ground_args_t *ga, ground_args_t *parent_ga)
{
    pddl_strips_op_t *parent;

    // If the operator corresponds to a conditional effect the
    // parent must be known already, because this is the way we
    // sorted ground_args_t structures.
    ASSERT_RUNTIME(parent_ga != NULL);

    // If parent action is not created then it had to have empty
    // effects. Therefore, we need to create the parent first.
    if (parent_ga->op_id == -1){
        pddl_strips_op_t op2;
        pddlStripsOpInit(&op2);
        setUpOp(g, &op2, parent_ga);
        parent_ga->op_id = pddlStripsOpsAdd(&g->strips->op, &op2);
        pddlStripsOpFree(&op2);
    }
    parent = g->strips->op.op[parent_ga->op_id];

    // Find out preconditions that belong only to the conditional
    // effect.
    borISetMinus(&op->pre, &parent->pre);
    if (op->pre.size > 0){
        // Create conditional effect if necessary
        pddlStripsOpAddCondEff(parent, op);
        g->strips->has_cond_eff = 1;

    }else{
        // If precondition of the conditional effect is empty, then
        // we can merge conditional effect directly to the parent
        // operator.
        // The operators are hashed only using its name so we can
        // merge effects without re-inserting operator.
        pddlStripsOpAddEffFromOp(parent, op);

        // If operator was well-formed before it must remain
        // well-formed.
        ASSERT(parent->add_eff.size > 0 || parent->del_eff.size > 0);
    }
}

static int groundActions(ground_t *g)
{
    ground_args_t *ga, *parent_ga;
    const pddl_prep_action_t *a;
    pddl_strips_op_t op;

    // Sorts unified arguments for actions so that conditional effects are
    // placed right after their respective non-cond-eff parent action.
    groundArgsSortAndUniq(&g->ground_args);

    parent_ga = NULL;
    for (int i = 0; i < g->ground_args.size; ++i){
        ga = g->ground_args.arg + i;
        a = ga->action;
        ASSERT(pddlPrepActionCheck(a, &g->static_fact, ga->arg));

        pddlStripsOpInit(&op);
        if (setUpOp(g, &op, ga) != 0){
            pddlStripsOpFree(&op);
            TRACE_RET(-1);
        }

        // Remember this action as a parent for conditional effects
        if (a->parent_action < 0)
            parent_ga = ga;

        // Use only operators with effects
        if (op.add_eff.size > 0 || op.del_eff.size > 0){
            if (a->parent_action >= 0){
                groundCondEff(g, &op, ga, parent_ga);
            }else{
                ga->op_id = pddlStripsOpsAdd(&g->strips->op, &op);
            }
        }

        pddlStripsOpFree(&op);
    }

    return 0;
}

static int groundInitState(ground_t *g)
{
    const pddl_fact_t *fact;
    int fact_id;

    for (int i = 0; i < g->pddl->init_fact.fact_size; ++i){
        fact = g->pddl->init_fact.fact[i];
        fact_id = pddlFactsFind(&g->strips->fact, fact);
        // Skipping static facts
        if (fact_id >= 0)
            borISetAdd(&g->strips->init, fact_id);
    }

    return 0;
}

struct ground_goal {
    ground_t *g;
    int fail;
};

static int _groundGoal(pddl_cond_t *c, void *_g)
{
    struct ground_goal *ggoal = _g;
    ground_t *g = ggoal->g;

    if (c->type == PDDL_COND_ATOM){
        const pddl_cond_atom_t *atom = PDDL_COND_CAST(c, atom);
        PDDL_FACT_FOR_GROUND2(fact, atom->arg_size);

        // Transform atom to a fact
        fact.pred = atom->pred;
        fact.arg_size = atom->arg_size;
        for (int i = 0; i < atom->arg_size; ++i){
            if (atom->arg[i].param >= 0){
                ERR_RET2(-1, "Goal specification cannot contain"
                             " parametrized atoms.");
            }else{
                fact.arg[i] = atom->arg[i].obj;
            }
        }

        // Find fact in the set of reachable facts
        int fact_id = pddlFactsFind(&g->strips->fact, &fact);
        if (fact_id >= 0){
            // Add the fact to the goal specification
            borISetAdd(&g->strips->goal, fact_id);
        }else{
            // The problem is unsolvable, because a goal fact is not
            // reachable.
            g->strips->goal_is_unreachable = 1;
        }
        return 0;

    }else if (c->type == PDDL_COND_AND){
        return 0;
    }else{
        ERR2("Only conjuctive goal specifications are supported.");
        ggoal->fail = 1;
        return -2;
    }
}
static int groundGoal(ground_t *g)
{
    struct ground_goal ggoal = { g, 0 };
    if (g->pddl->goal->type == PDDL_COND_OR)
        ERR_RET2(-1, "Only conjuctive goal specifications are supported.");

    pddlCondTraverse(g->pddl->goal, _groundGoal, NULL, &ggoal);
    if (ggoal.fail)
        TRACE_RET(-1);
    return 0;
}

static void groundInitFact(ground_t *g, const pddl_t *pddl)
{
    const pddl_fact_t *fact;

    for (int i = 0; i < pddl->init_fact.fact_size; ++i){
        fact = pddl->init_fact.fact[i];
        if (pddlFactIsStatic(pddl, fact)){
            pddlFactsAdd(&g->static_fact, fact);
        }else{
            pddlFactsAdd(&g->strips->fact, fact);
        }
    }
}

static int groundInit(ground_t *g, pddl_strips_t *strips, const pddl_t *pddl)
{
    bzero(g, sizeof(*g));
    g->strips = strips;
    g->pddl = pddl;

    if (pddlPrepActionsInit(pddl, &g->action) != 0)
        TRACE_RET(-1);

    pddlFactsInit(&g->static_fact);

    groundInitFact(g, pddl);

    g->tree = BOR_ALLOC_ARR(tree_t, g->action.size);
    for (int i = 0; i < g->action.size; ++i)
        treeInit(g->tree + i, g, i);

    return 0;
}

static void groundFree(ground_t *g)
{
    for (int i = 0; i < g->action.size; ++i)
        treeFree(g->tree + i);
    if (g->tree != NULL)
        BOR_FREE(g->tree);
    pddlFactsFree(&g->static_fact);
    pddlPrepActionsFree(&g->action);
    groundArgsFree(&g->ground_args);
}

int _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl)
{
    ground_t g;

    if (groundInit(&g, strips, pddl) != 0
            || unifyStaticFacts(&g) != 0
            || unifyFacts(&g) != 0
            || groundActions(&g) != 0
            || groundInitState(&g) != 0
            || groundGoal(&g) != 0){
        groundFree(&g);
        TRACE_RET(-1);
    }

    groundFree(&g);
    return 0;
}

#ifdef PDDL_DEBUG
static void tnodePrint(tree_t *tr, tnode_t *tn, int offset, FILE *fout)
{
    tnode_t *ch;
    int off = 0, p = 0;

    off += fprintf(fout, "%d", tn->argi);
    if (tn->obj_id == UNDEF){
        off += fprintf(fout, ":X");
    }else{
        off += fprintf(fout, ":%d", tn->obj_id);
    }
#ifdef PDDL_DEBUG
    off += fprintf(fout, ":M%x", tn->pre_mask);
#endif /* PDDL_DEBUG */
    off += fprintf(fout, ":P%d", tn->pre_unified);
    if (tn->flags.blocked)
        off += fprintf(fout, ":B");
    if (tn->flags.pre_unified)
        off += fprintf(fout, ":U");
    if (tn->flags.static_arg)
        off += fprintf(fout, ":S");
    if (tn->pre_unified == tr->pre_size)
        off += fprintf(fout, "*");

    TNODE_FOR_EACH_CHILD(tn, ch){
        if (p){
            fprintf(fout, "\n");
            for (int i = 0; i < offset + off; ++i)
                fprintf(fout, " ");
            fprintf(fout, "`");
        }else{
            fprintf(fout, " ");
            p = 1;
        }
        tnodePrint(tr, ch, offset + off + 1, fout);
    }

    if (offset == 0)
        fprintf(fout, "\n");
}

static void treePrint(tree_t *tr, FILE *fout)
{
    tnode_t *ch;

    fprintf(fout, "Tree for %s, arg_size: %d, pre_size: %d, "
                  " pre_static_size: %d, root-blocked: %d, param-size:",
            tr->action->action->name, tr->arg_size, tr->pre_size,
            tr->pre_static_size, tr->root->flags.blocked);
    for (int i = 0; i < tr->arg_size; ++i)
        fprintf(fout, " %d:%d", i, tr->arg_max_size[i]);
    fprintf(fout, "\n");
    TNODE_FOR_EACH_CHILD(tr->root, ch)
        tnodePrint(tr, ch, 0, fout);
}
#endif /* PDDL_DEBUG */
