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
};
typedef struct tnode_child tnode_child_t;

struct tnode {
    obj_id_t obj_id;
    pre_mask_t pre_mask; /*!< Bits set on positions where precondition is set */
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

struct trie {
    struct ground *g;
    int action_id;
    const pddl_prep_action_t *action;
    int arg_size;
    int pre_size;
    pre_mask_t pre_mask;
    pred_to_pre_t *pred_to_pre;
    tnode_t *root;
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
static void groundActionAddEff(ground_t *g,
                               const pddl_prep_action_t *a,
                               const obj_id_t *oarg);

static tnode_t *tnodeNew(trie_t *t, tnode_t *parent, obj_id_t obj_id)
{
    tnode_t *n;
    size_t size;

    size = sizeof(*n) + sizeof(tnode_child_t) * t->arg_size;
    n = BOR_MALLOC(size);
    bzero(n, size);
    n->obj_id = obj_id;
    if (parent != NULL)
        n->pre_mask = parent->pre_mask;
    return n;
}

static void tnodeDel(trie_t *tr, tnode_t *t);
static void tnodeFree(trie_t *tr, tnode_t *t)
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

static void tnodeDel(trie_t *tr, tnode_t *t)
{
    tnodeFree(tr, t);
    BOR_FREE(t);
}

static void tnodeReserveChild(trie_t *tr, tnode_t *n, int argi)
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

static void tnodeAddChildPtr(trie_t *t, tnode_t *par, int argi, tnode_t *add)
{
    tnode_child_t *cs = par->child + argi;

    tnodeReserveChild(t, par, argi);
    cs->child[cs->child_size++] = add;

    // sort childs according to .obj_id
    _tnodeChildBubbleDown(par, argi, cs->child_size - 1);
}

static tnode_t *tnodeAddChild(trie_t *t, tnode_t *par, int argi, obj_id_t obj_id)
{
    tnode_t *n = tnodeNew(t, par, obj_id);
    tnodeAddChildPtr(t, par, argi, n);
    return n;
}

static void tnodeDelChild(trie_t *tr, tnode_t *par, tnode_t *ch, int argi)
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

static void unifyPre(trie_t *tr, tnode_t *tn, obj_id_t *arg, int pre_i)
{
    tnode_child_t *cs;
    int child_num;

    ASSERT(!(tn->pre_mask & (1u << ((pre_mask_t)pre_i))));

    // TODO: Check action for equality and predicates?
    PRE_MASK_SET(tn, pre_i);

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
    if (tn->pre_mask == tr->pre_mask && child_num == 0){
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
        groundActionAddEff(tr->g, tr->action, arg);
    }
}

static tnode_t *unifyNew(trie_t *tr, tnode_t *tn, obj_id_t *arg,
                         int remain, const obj_id_t *arg_pre, int pre_i)
{
    tnode_t *new = NULL;

    for (int i = 0; i < tr->arg_size; ++i){
        if (arg[i] == UNDEF && arg_pre[i] != UNDEF){
            arg[i] = arg_pre[i];
            new = tnodeAddChild(tr, tn, i, arg[i]);
            if (remain - 1 > 0){
                unifyNew(tr, new, arg, remain - 1, arg_pre, pre_i);
            }else{
                unifyPre(tr, new, arg, pre_i);
            }
            arg[i] = UNDEF;
            return new;
        }
    }

    ASSERT(new != NULL);
    return new;
}

static void unify(trie_t *tr, tnode_t *tn, obj_id_t *arg, int remain,
                  const obj_id_t *pre_arg, int pre_i, tnode_t *clone);
static void unifyOrClone(trie_t *tr, tnode_t *ch, int argi,
                         obj_id_t *arg, int remain,
                         const obj_id_t *arg_pre, int pre_i, tnode_t *clone)
{
    tnode_t *cnew;

    if (clone != NULL){
        cnew = tnodeAddChild(tr, clone, argi, arg[argi]);
        cnew->pre_mask |= ch->pre_mask;
        unify(tr, ch, arg, remain, arg_pre, pre_i, cnew);
    }else{
        unify(tr, ch, arg, remain, arg_pre, pre_i, NULL);
    }
}

static int unifyArg(trie_t *tr, tnode_t *tn, int argi,
                    obj_id_t *arg, int remain,
                    const obj_id_t *arg_pre, int pre_i, tnode_t *clone)
{
    tnode_child_t *tnc = tn->child + argi;
    tnode_t *ch, *trace = NULL, *cnew;
    int match = 0;

    arg[argi] = arg_pre[argi];

    tnc = tn->child + argi;
    for (int i = 0; i < tnc->child_size; ++i){
        ch = tnc->child[i];

        if (ch->obj_id == arg[argi]){
            ASSERT(!(ch->pre_mask & (1u << pre_i)));
            // Found exact match on the argument
            if (arg[argi] != UNDEF){
                unifyOrClone(tr, ch, argi, arg, remain - 1,
                             arg_pre, pre_i, clone);
            }else{
                unifyOrClone(tr, ch, argi, arg, remain, arg_pre, pre_i, clone);
            }
            if (ch->obj_id != UNDEF)
                match = 1;

        }else if (arg[argi] != UNDEF && ch->obj_id == UNDEF){
            // If we will need to create a new subtree, we have to trace
            // this node for unified preconditions.
            trace = ch;

        }else if (arg[argi] == UNDEF && ch->obj_id != UNDEF){
            ASSERT(!(ch->pre_mask & (1u << pre_i)));
            // Argument is not set therefore we need to unify with all set
            // arguments
            arg[argi] = ch->obj_id;
            unifyOrClone(tr, ch, argi, arg, remain, arg_pre, pre_i, clone);
            arg[argi] = UNDEF;
        }
    }

    if (!match && trace != NULL){
        if (clone == NULL){
            clone = tnodeAddChild(tr, tn, argi, arg[argi]);
            ASSERT((clone->pre_mask & trace->pre_mask) == trace->pre_mask);
            // TODO: Remove the line below
            clone->pre_mask |= trace->pre_mask;
            if (arg[argi] != UNDEF){
                unify(tr, trace, arg, remain - 1, arg_pre, pre_i, clone);
            }else{
                unify(tr, trace, arg, remain, arg_pre, pre_i, clone);
            }
        }else{
            cnew = tnodeAddChild(tr, clone, argi, arg[argi]);
            ASSERT((cnew->pre_mask & trace->pre_mask) == trace->pre_mask);
            // TODO: Remove the line below
            cnew->pre_mask |= trace->pre_mask;
            if (arg[argi] != UNDEF){
                unify(tr, trace, arg, remain - 1, arg_pre, pre_i, cnew);
            }else{
                unify(tr, trace, arg, remain, arg_pre, pre_i, cnew);
            }
        }

        /*
    }else if (!match){
        // TODO
        // If we haven't found a matching argument we needd to create a
        // whole new subtree.
        if (clone == NULL){
            ch = unifyNew(tr, tn, arg, remain, arg_pre, pre_i);
        }else{
            ch = unifyNew(tr, clone, arg, remain, arg_pre, pre_i);
        }
        unifyPre(tr, ch, arg, pre_i);
        */
    }

    arg[argi] = UNDEF;
    return match;
}

static void unify(trie_t *tr, tnode_t *tn, obj_id_t *arg, int remain,
                  const obj_id_t *arg_pre, int pre_i, tnode_t *clone)
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
            match |= unifyArg(tr, tn, argi, arg, remain, arg_pre, pre_i, clone);
    }

    if (!match){
        if (clone == NULL){
            unifyNew(tr, tn, arg, remain, arg_pre, pre_i);
        }else{
            unifyNew(tr, clone, arg, remain, arg_pre, pre_i);
        }
    }
}

static void trieUnify(trie_t *tr, const pddl_fact_t *fact, int pre_i)
{
    const pddl_cond_atom_t *atom;
    obj_id_t arg[tr->arg_size], arg_pre[tr->arg_size];
    int num_args_set = 0;
    int param;

    fprintf(stderr, "Fact: ");
    pddlFactPrint(tr->g->pddl, fact, stderr);
    fprintf(stderr, " --> %s, pre_i: %d\n", tr->action->action->name, pre_i);
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
        if (param >= 0){
            arg_pre[param] = fact->arg[i];
            ++num_args_set;
        }
    }

    fprintf(stderr, "Fact: ");
    pddlFactPrint(tr->g->pddl, fact, stderr);
    for (int i = 0; i < tr->arg_size; ++i)
        fprintf(stderr, " %d", arg_pre[i]);
    fprintf(stderr, " | pre_i: %d\n", pre_i);
    unify(tr, tr->root, arg, num_args_set, arg_pre, pre_i, NULL);
    triePrint(tr, stderr);
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
    tr->root = tnodeNew(tr, NULL, UNDEF);

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
    tnodeDel(tr, tr->root);
}

static void tnodePrint(trie_t *tr, tnode_t *tn, int argi, int offset, FILE *fout)
{
    int off = 0, p = 0;

    off += fprintf(fout, "%d", argi);
    if (tn->obj_id == UNDEF){
        off += fprintf(fout, ":U");
    }else{
        off += fprintf(fout, ":%d", tn->obj_id);
    }
    off += fprintf(fout, "|%x", tn->pre_mask);
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

static void triePrint(trie_t *tr, FILE *fout)
{
    fprintf(fout, "Trie for %s, arg_size: %d, pre_size: %d, pre_mask: %x\n",
            tr->action->action->name, tr->arg_size, tr->pre_size,
            tr->pre_mask);
    for (int argi = 0; argi < tr->arg_size; ++argi){
        tnode_child_t *cs = tr->root->child + argi;
        for (int i = 0; i < cs->child_size; ++i)
            tnodePrint(tr, cs->child[i], argi, 0, fout);
    }
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

        fprintf(stderr, "ADD FACT ");
        pddlFactPrint(g->pddl, &fact, stderr);
        fprintf(stderr, "\n");
    }
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

void __pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl)
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

    fprintf(stderr, "END:\n");
    for (int j = 0; j < g.action.size; ++j){
        trie_t *tr = g.trie + j;
        triePrint(tr, stderr);
    }

    groundFree(&g);
}
