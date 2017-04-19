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

struct tnode {
    obj_id_t obj_id;
    pre_mask_t pre_mask; /*!< Bits set on positions where precondition is set */
    obj_id_t child_size;
    obj_id_t child_alloc;
    struct tnode **child;
} bor_packed;
typedef struct tnode tnode_t;

#define PRE_MASK_SET(N, IDX) \
    ((N)->pre_mask = (N)->pre_mask | (((pre_mask_t)1u) << (pre_mask_t)(IDX)))

struct pred_to_pre {
    int *pre;
    int size;
};
typedef struct pred_to_pre pred_to_pre_t;

struct trie {
    struct ground *g;
    int action_id;
    const pddl_prep_action_t *action;
    int arg_size;
    int pre_size;
    pre_mask_t pre_mask;
    pred_to_pre_t *pred_to_pre;
    tnode_t root;
};
typedef struct trie trie_t;

struct ground {
    const pddl_t *pddl;
    pddl_prep_actions_t action;
    pddl_facts_t static_fact;
    pddl_facts_t fact;
    pddl_strips_ops_t op;
    trie_t *trie;
};
typedef struct ground ground_t;

static void triePrint(trie_t *tr, FILE *fout);

static tnode_t *tnodeNew(trie_t *t, tnode_t *parent, obj_id_t obj_id)
{
    tnode_t *n;

    n = BOR_MALLOC(sizeof(*n));
    bzero(n, sizeof(*n));
    n->obj_id = obj_id;
    n->pre_mask = parent->pre_mask;
    return n;
}

static void tnodeDel(tnode_t *t);
static void tnodeFree(tnode_t *t)
{
    for (int i = 0; i < t->child_size; ++i){
        if (t->child[i] != NULL)
            tnodeDel(t->child[i]);
    }
    if (t->child != NULL)
        BOR_FREE(t->child);
}

static void tnodeDel(tnode_t *t)
{
    tnodeFree(t);
    BOR_FREE(t);
}

static void tnodeReserveChild(tnode_t *n)
{
    if (n->child_size == n->child_alloc){
        if (n->child_alloc == 0)
            n->child_alloc = 1;
        n->child_alloc *= 2;
        n->child = BOR_REALLOC_ARR(n->child, tnode_t *, n->child_alloc);
    }
}

static void _tnodeChildBubbleDown(tnode_t *tn, int idx)
{
    // sort childs according to .obj_id
    for (tnode_t **c = tn->child + idx - 1;
            c >= tn->child && c[0]->obj_id > c[1]->obj_id; --c){
        tnode_t *s = c[0];
        c[0] = c[1];
        c[1] = s;
    }
}

static void tnodeAddChildPtr(trie_t *t, tnode_t *par, tnode_t *add)
{
    tnodeReserveChild(par);
    par->child[par->child_size++] = add;

    // sort childs according to .obj_id
    _tnodeChildBubbleDown(par, par->child_size - 1);
}

static tnode_t *tnodeAddChild(trie_t *t, tnode_t *par, obj_id_t obj_id)
{
    tnode_t *n = tnodeNew(t, par, obj_id);
    tnodeAddChildPtr(t, par, n);
    return n;
}

static void tnodeDelChild(trie_t *tr, tnode_t *par, tnode_t *ch, int argi)
{
    int i;

    // TODO: binary search?
    for (i = 0; i < par->child_size; ++i){
        if (par->child[i] == ch){
            tnodeDel(ch);
            break;
        }
    }
    for (++i; i < par->child_size; ++i)
        par->child[i - 1] = par->child[i];
    par->child_size = i - 1;
}

static void tnodeUnifyPre(trie_t *tr, tnode_t *tn, int pre_i,
                          int argi, obj_id_t *arg)
{
    ASSERT(!(tn->pre_mask & (1u << ((pre_mask_t)pre_i))));

    // TODO: Check action for equality and predicates?
    PRE_MASK_SET(tn, pre_i);

    // If all preconditions are unified and we are at leaf node we can
    // ground the action using assigned arguments.
    if (tn->pre_mask == tr->pre_mask && tn->child_size == 0){
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
        //       memory removing part of trie. The question is whether is
        //       it useful.
    }

    // Recursivelly unify predicate with the whole subtree
    for (int i = 0; i < tn->child_size; ++i){
        ASSERT(argi < tr->arg_size);
        ASSERT(arg[argi] == UNDEF);
        arg[argi] = tn->child[i]->obj_id;
        tnodeUnifyPre(tr, tn->child[i], pre_i, argi + 1, arg);
        arg[argi] = UNDEF;
    }
}

static void tnodeUnifyCloneSubtree(trie_t *tr, tnode_t *par,
                                   int argi, obj_id_t *arg,
                                   const tnode_t *trace)
{
    tnode_t *ch, *newn;

    // TODO: Check equality between arg[argi] and arg[0..argi-1]?
    for (int i = 0; i < trace->child_size; ++i){
        ch = trace->child[i];
        ASSERT(arg[argi] == UNDEF);
        arg[argi] = ch->obj_id;
        newn = tnodeAddChild(tr, par, ch->obj_id);
        newn->pre_mask |= ch->pre_mask;
        tnodeUnifyCloneSubtree(tr, newn, argi + 1, arg, ch);
        arg[argi] = UNDEF;
    }
}

static void tnodeUnifyNew(trie_t *tr, tnode_t *tn,
                          int argi, obj_id_t *arg, int max_argi, int pre_i)
{
    tnode_t *new;

    if (argi > max_argi){
        // Unify precondition
        tnodeUnifyPre(tr, tn, pre_i, argi, arg);
    }else{
        new = tnodeAddChild(tr, tn, arg[argi]);
        tnodeUnifyNew(tr, new, argi + 1, arg, max_argi, pre_i);
    }
}

static void tnodeUnify(trie_t *tr, tnode_t *tn, int argi,
                       obj_id_t *arg, int max_argi, int pre_i,
                       tnode_t *clone)
{
    tnode_t *ch, *trace = NULL, *cnew;
    int match = 0;

    if (argi > max_argi){
        // We were able to match arguments completely, so the precondition
        // can be unified.
        if (clone == NULL){
            tnodeUnifyPre(tr, tn, pre_i, argi, arg);
        }else{
            tnodeUnifyPre(tr, clone, pre_i, argi, arg);
        }
        return;
    }

    for (int i = 0; i < tn->child_size; ++i){
        ch = tn->child[i];
        ASSERT(!(ch->pre_mask & (1u << pre_i)));

        if (ch->obj_id == arg[argi]){
            // Found exact match on the argument
            if (clone != NULL){
                cnew = tnodeAddChild(tr, clone, arg[argi]);
                cnew->pre_mask |= ch->pre_mask;
                tnodeUnify(tr, ch, argi + 1, arg, max_argi, pre_i, cnew);
            }else{
                tnodeUnify(tr, ch, argi + 1, arg, max_argi, pre_i, NULL);
            }
            match = 1;

        }else if (arg[argi] != UNDEF && ch->obj_id == UNDEF){
            // If we will need to create a new subtree, we have to trace
            // this node for unified preconditions.
            trace = ch;

        }else if (arg[argi] == UNDEF && ch->obj_id != UNDEF){
            // Argument is not set therefore we need to unify with all set
            // arguments
            arg[argi] = ch->obj_id;
            // TODO: refactor with the above
            if (clone != NULL){
                cnew = tnodeAddChild(tr, clone, arg[argi]);
                cnew->pre_mask |= ch->pre_mask;
                tnodeUnify(tr, ch, argi + 1, arg, max_argi, pre_i, cnew);
            }else{
                tnodeUnify(tr, ch, argi + 1, arg, max_argi, pre_i, NULL);
            }
            arg[argi] = UNDEF;
        }
    }

    if (!match && trace != NULL){
        if (clone == NULL){
            clone = tnodeAddChild(tr, tn, arg[argi]);
            ASSERT((clone->pre_mask & trace->pre_mask) == trace->pre_mask);
            // TODO: Remove the line below
            clone->pre_mask |= trace->pre_mask;
            tnodeUnify(tr, trace, argi + 1, arg, max_argi, pre_i, clone);
        }else{
            cnew = tnodeAddChild(tr, clone, arg[argi]);
            ASSERT((cnew->pre_mask & trace->pre_mask) == trace->pre_mask);
            // TODO: Remove the line below
            cnew->pre_mask |= trace->pre_mask;
            tnodeUnify(tr, trace, argi + 1, arg, max_argi, pre_i, cnew);
        }

    }else if (!match){
        // If we haven't found a matching argument we needd to create a
        // whole new subtree.
        if (clone == NULL){
            tnodeUnifyNew(tr, tn, argi, arg, max_argi, pre_i);
        }else{
            tnodeUnifyNew(tr, clone, argi, arg, max_argi, pre_i);
        }
    }
}

static void predToPreAdd(pred_to_pre_t *p, int pre_id)
{
    ++p->size;
    p->pre = BOR_REALLOC_ARR(p->pre, int, p->size);
    p->pre[p->size - 1] = pre_id;
}

static void trieInit(trie_t *tr, ground_t *g, int action_id)
{
    pddl_prep_action_t *a = g->action.action + action_id;
    const pddl_cond_atom_t *atom;

    // TODO: Check limits on obj_id_t, pre_mask_t, ...

    bzero(tr, sizeof(*tr));
    tr->g = g;
    tr->action_id = action_id;
    tr->action = a;
    tr->arg_size = a->param_size;
    tr->pre_size = a->pre.size;

    for (int i = 0; i < tr->pre_size; ++i)
        tr->pre_mask = (tr->pre_mask << 1u) | 1u;

    tr->pred_to_pre = BOR_CALLOC_ARR(pred_to_pre_t, g->pddl->pred.size);
    for (int i = 0; i < a->pre.size; ++i){
        atom = PDDL_COND_CAST(a->pre.cond[i], atom);
        predToPreAdd(tr->pred_to_pre + atom->pred, i);
    }
}

static void trieFree(trie_t *tr)
{
    for (int i = 0; i < tr->g->pddl->pred.size; ++i){
        if (tr->pred_to_pre[i].pre != NULL)
            BOR_FREE(tr->pred_to_pre[i].pre);
    }
    if (tr->pred_to_pre != NULL)
        BOR_FREE(tr->pred_to_pre);
    tnodeFree(&tr->root);
}

static void trieUnify(trie_t *tr, const pddl_fact_t *fact, int pre_i)
{
    const pddl_cond_atom_t *atom;
    obj_id_t arg[tr->arg_size];
    int max_argi = 0;

    fprintf(stderr, "Fact: ");
    pddlFactPrint(tr->g->pddl, fact, stderr);
    fprintf(stderr, " --> %s\n", tr->action->action->name);
    // TODO: check fact agains action
    // TODO: Static facts -- after using all of them disallow -1 on
    //       arguments of static facts.
    // TODO: Remove -1 nodes if all possible objects are already present
    //       and lock the argument
    if (!pddlPrepActionCheckPre(tr->g->pddl, tr->action, pre_i, fact))
        return;

    atom = PDDL_COND_CAST(tr->action->pre.cond[pre_i], atom);
    for (int i = 0; i < tr->arg_size; ++i)
        arg[i] = UNDEF;
    for (int i = 0; i < atom->arg_size; ++i){
        if (atom->arg[i].param >= 0){
            arg[atom->arg[i].param] = fact->arg[i];
            max_argi = BOR_MAX(max_argi, atom->arg[i].param);
        }
    }

    tnodeUnify(tr, &tr->root, 0, arg, max_argi, pre_i, NULL);
    fprintf(stderr, "Fact: ");
    pddlFactPrint(tr->g->pddl, fact, stderr);
    for (int i = 0; i < tr->arg_size; ++i)
        fprintf(stderr, " %d", arg[i]);
    fprintf(stderr, "\n");
    triePrint(tr, stderr);
}

static void tnodePrint(trie_t *tr, tnode_t *tn, int offset, FILE *fout)
{
    int off = 0;
    if (tn->obj_id == UNDEF){
        off += fprintf(fout, "U:");
    }else{
        off += fprintf(fout, "%d:", tn->obj_id);
    }
    off += fprintf(fout, "%x", tn->pre_mask);
    if (tn->pre_mask == tr->pre_mask)
        off += fprintf(fout, "*");

    for (int i = 0; i < tn->child_size; ++i){
        if (i > 0){
            fprintf(fout, "\n");
            for (int i = 0; i < offset + off; ++i)
                fprintf(fout, " ");
            fprintf(fout, "`");
        }else{
            fprintf(fout, " ");
        }
        tnodePrint(tr, tn->child[i], offset + off + 1, fout);
    }

    if (offset == 0)
        fprintf(fout, "\n");
}

static void triePrint(trie_t *tr, FILE *fout)
{
    fprintf(fout, "Trie for %s, arg_size: %d, pre_size: %d, pre_mask: %x\n",
            tr->action->action->name, tr->arg_size, tr->pre_size,
            tr->pre_mask);
    for (int i = 0; i < tr->root.child_size; ++i)
        tnodePrint(tr, tr->root.child[i], 0, fout);
}


static void groundInitStaticFact(ground_t *g, const pddl_t *pddl)
{
    const pddl_fact_t *fact;

    for (int i = 0; i < pddl->init_fact.fact_size; ++i){
        fact = pddl->init_fact.fact[i];
        if (pddlFactIsStatic(pddl, fact)){
            pddlFactsAdd(&g->static_fact, fact);
            pddlFactsAdd(&g->fact, fact);
        }
    }

    if (pddl->pred.eq_pred >= 0){
        PDDL_FACT_FOR_GROUND2(eq_fact, 2);
        for (int i = 0; i < pddl->obj.size; ++i){
            eq_fact.pred = pddl->pred.eq_pred;
            eq_fact.arg_size = 2;
            eq_fact.arg[0] = i;
            eq_fact.arg[1] = i;
            pddlFactsAdd(&g->static_fact, &eq_fact);
            pddlFactsAdd(&g->fact, &eq_fact);
        }
    }
}

static void groundInitFact(ground_t *g, const pddl_t *pddl)
{
    const pddl_fact_t *fact;

    for (int i = 0; i < pddl->init_fact.fact_size; ++i){
        fact = pddl->init_fact.fact[i];
        if (!pddlFactIsStatic(pddl, fact)){
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

    groundInitStaticFact(g, pddl);
    groundInitFact(g, pddl);

    g->trie = BOR_ALLOC_ARR(trie_t, g->action.size);
    for (int i = 0; i < g->action.size; ++i)
        trieInit(g->trie + i, g, i);
}

static void groundFree(ground_t *g)
{
    for (int i = 0; i < g->action.size; ++i)
        trieFree(g->trie + i);
    if (g->trie != NULL)
        BOR_FREE(g->trie);
    pddlStripsOpsFree(&g->op);
    pddlFactsFree(&g->fact);
    pddlFactsFree(&g->static_fact);
    pddlPrepActionsFree(&g->action);
}

void _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl)
{
    ground_t g;

    groundInit(&g, pddl);

    for (int i = 0; i < g.fact.fact_size; ++i){
        const pddl_fact_t *fact = g.fact.fact[i];
        fprintf(stderr, "Pop Fact: ");
        pddlFactPrint(g.pddl, fact, stderr);
        fprintf(stderr, " \n");

        for (int j = 0; j < g.action.size; ++j){
            trie_t *tr = g.trie + j;
            for (int k = 0; k < tr->pred_to_pre[fact->pred].size; ++k){
                trieUnify(tr, fact, tr->pred_to_pre[fact->pred].pre[k]);
            }
        }
    }

    for (int j = 0; j < g.action.size; ++j){
        trie_t *tr = g.trie + j;
        triePrint(tr, stderr);
    }

    groundFree(&g);
}
