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

#include <boruvka/alloc.h>
#include <boruvka/htable.h>
#include <boruvka/hfunc.h>
#include <boruvka/sort.h>
#include "pddl/strips.h"
#include "pddl/prep_action.h"
#include "err.h"

// TODO: Replace -1 by defined of type obj_id_t constant

typedef int obj_id_t;
typedef uint32_t pre_size_t;
typedef uint32_t arg_size_t;

typedef uint32_t pre_mask_t;
typedef uint32_t arg_mask_t;
typedef uint64_t pred_mask_t;

struct ground;

struct tnode {
    obj_id_t obj_id;
    arg_mask_t arg_mask; /*!< Bits set on positions where argument is set */
    pre_mask_t pre_mask; /*!< Bits set on positions where precondition is set */
    obj_id_t child_size;
    obj_id_t child_alloc;
    struct tnode **child;
} bor_packed;
typedef struct tnode tnode_t;

#define ARG_MASK_SET(N, IDX) \
    ((N)->arg_mask = (N)->arg_mask | (((arg_mask_t)1u) << (arg_mask_t)(IDX)))
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
    arg_size_t arg_mask;
    pre_size_t pre_mask;
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

static tnode_t *tnodeNew(trie_t *t, tnode_t *parent,
                         int argi, obj_id_t obj_id)
{
    tnode_t *n;
    size_t size;

    if (argi == t->arg_size - 1){
        // Allocate only the part that will be used in leaf nodes
        size = bor_offsetof(tnode_t, child_size);
    }else{
        size = sizeof(*n);
    }
    n = BOR_MALLOC(size);
    bzero(n, size);
    n->obj_id = obj_id;
    n->arg_mask = parent->arg_mask;
    n->pre_mask = parent->pre_mask;
    ARG_MASK_SET(n, argi);
    return n;
}

static void tnodeDel(tnode_t *t)
{
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

static tnode_t *tnodeAddChild(trie_t *t, tnode_t *par,
                              int arg_id, obj_id_t obj_id)
{
    tnode_t *n = tnodeNew(t, par, arg_id, obj_id);
    tnodeReserveChild(par);
    par->child[par->child_size++] = n;

    // sort childs according to .obj_id
    for (tnode_t **c = par->child + par->child_size - 2;
            c >= par->child && c[0]->obj_id > c[1]->obj_id; --c){
        tnode_t *s = c[0];
        c[0] = c[1];
        c[1] = s;
    }

    return n;
}

static void tnodeUnifyPre(trie_t *tr, tnode_t *tn, int pre_i, const obj_id_t *arg)
{
    // TODO: Replace by assert
    if ((tn->pre_mask & (1u << ((pre_mask_t)pre_i))) != 0){
        fprintf(stderr, "ERR BOUND PRE\n");
        exit(-1);
    }

    PRE_MASK_SET(tn, pre_i);
    // TODO: If all preconditions are unified, ground its effects using arg

    for (int i = 0; i < tn->child_size; ++i)
        tnodeUnifyPre(tr, tn->child[i], pre_i, arg);
}

static void tnodeUnifyNew(trie_t *tr, tnode_t *par,
                          int argi, obj_id_t *arg, int max_argi, int pre_i,
                          const tnode_t *trace)
{
    tnode_t *tn, *ch, *trace_ch = NULL, *trace_part_ch = NULL;

    if (argi > max_argi){
        // The new candidate is finalized, we can unify precondition
        PRE_MASK_SET(par, pre_i);

        // Now we can clone the whole subtree of trace if the additional
        // grounding makes sense.
        // TODO
        return;
    }

    tn = tnodeAddChild(tr, par, argi, arg[argi]);
    if (trace != NULL && arg[argi] == -1){
        // TODO: if arg[argi] == -1 then any trace->child[i] != -1 must be
        //       used for a creation of a new path
        for (int i = 0; i < trace->child_size; ++i){
            ch = trace->child[i];
            if (ch->obj_id != -1){
                // TODO
            }
        }
    }else if (trace != NULL){
        for (int i = 0; i < trace->child_size; ++i){
            // TODO: We can use binary search if we keep sorted childs
            ch = trace->child[i];
            if (ch->obj_id == arg[argi]){
                trace_ch = ch;
                break;
            }else if (ch->obj_id == -1){
                trace_part_ch = ch;
            }
        }

        if (!trace_ch)
            trace_ch = trace_part_ch;

        trace = NULL;
        if (trace_ch){
            tn->pre_mask |= trace_ch->pre_mask;
            trace = trace_ch;
        }
    }

    tnodeUnifyNew(tr, tn, argi + 1, arg, max_argi, pre_i, trace);
}

static void tnodeUnify(trie_t *tr, tnode_t *tn, int argi,
                       obj_id_t *arg, int max_argi, int pre_i)
{
    tnode_t *ch, *part_match = NULL;
    int match = 0;

    if (argi > max_argi){
        // This means that we were able to match arguments completely,
        // which means that arguments are already set and we need just
        // unify precondition
        tnodeUnifyPre(tr, tn, pre_i, arg);
        return;
    }

    for (int i = 0; i < tn->child_size; ++i){
        ch = tn->child[i];

        // Skip nodes where pre_i is already unified
        // TODO: I think we don't need this because it is implied by the
        //       arguments
        if (ch->pre_mask & (1u << pre_i))
            continue;

        if (ch->obj_id == arg[argi]){
            // Found exact match on the argument
            tnodeUnify(tr, ch, argi + 1, arg, max_argi, pre_i);
            match = 1;

        }else if (ch->obj_id == -1 && arg[argi] != -1){
            // Need to match against partially grounded action that has not
            // set this argument
            part_match = ch;

        }else if (arg[argi] == -1){
            // Argument is not set therefore any matching is admissible
            arg[argi] = ch->obj_id;
            tnodeUnify(tr, ch, argi + 1, arg, max_argi, pre_i);
            arg[argi] = -1;
        }
    }

    // Create a new path
    if (!match)
        tnodeUnifyNew(tr, tn, argi, arg, max_argi, pre_i, part_match);
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

    bzero(tr, sizeof(*tr));
    tr->g = g;
    tr->action_id = action_id;
    tr->action = a;
    tr->arg_size = a->param_size;
    tr->pre_size = a->pre.size;

    for (int i = 0; i < tr->arg_size; ++i)
        tr->arg_mask = (tr->arg_mask << 1u) | 1u;
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
    // TODO
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
        arg[i] = -1;
    for (int i = 0; i < atom->arg_size; ++i){
        if (atom->arg[i].param >= 0){
            arg[atom->arg[i].param] = fact->arg[i];
            max_argi = BOR_MAX(max_argi, atom->arg[i].param);
        }
    }

    tnodeUnify(tr, &tr->root, 0, arg, max_argi, pre_i);
    fprintf(stderr, "Fact: ");
    pddlFactPrint(tr->g->pddl, fact, stderr);
    for (int i = 0; i < tr->arg_size; ++i)
        fprintf(stderr, " %d", arg[i]);
    fprintf(stderr, "\n");
    triePrint(tr, stderr);
}

static void tnodePrint(trie_t *tr, tnode_t *tn, int level, FILE *fout)
{
    for (int i = 0; i < level; ++i)
        fprintf(fout, "  ");
    //fprintf(fout, "%d: obj: %d, arg_mask: %x, pre_mask: %x",
    //        level, tn->obj_id, tn->arg_mask, tn->pre_mask);
    fprintf(fout, "%d: arg_mask: %x, pre_mask: %x",
            tn->obj_id, tn->arg_mask, tn->pre_mask);
    if (tn->pre_mask == tr->pre_mask)
        fprintf(fout, "*");
    fprintf(fout, "\n");
    if (level < tr->arg_size){
        for (int i = 0; i < tn->child_size; ++i){
            tnodePrint(tr, tn->child[i], level + 1, fout);
        }
    }
}

static void triePrint(trie_t *tr, FILE *fout)
{
    fprintf(fout, "Trie for %s, arg_size: %d, pre_size: %d, arg_mask: %x,"
                  " pre_mask: %x\n",
            tr->action->action->name, tr->arg_size, tr->pre_size,
            tr->arg_mask, tr->pre_mask);
    tnodePrint(tr, &tr->root, 0, fout);
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
        for (int i = 0; i < pddl->obj.size; ++i){
            PDDL_FACT_FOR_GROUND2(eq_fact, 2);
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
    pddlPrepActionsFree(&g->action);
    pddlFactsFree(&g->static_fact);
    pddlFactsFree(&g->fact);
    pddlStripsOpsFree(&g->op);
}

void _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl)
{
    ground_t g;

    groundInit(&g, pddl);

    for (int i = 0; i < g.fact.fact_size; ++i){
        const pddl_fact_t *fact = g.fact.fact[i];
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

#if 0
static tnode_t *tnodeNew(trie_t *t, tnode_t *parent,
                         int arg_id, obj_id_t obj_id)
{
    tnode_t *n;
    size_t size;

    if (arg_id == t->arg_size - 1){
        // Allocate only the part that will be used in leaf nodes
        size = bor_offsetof(tnode_t, child_size);
    }else{
        size = sizeof(*n);
    }
    n = BOR_MALLOC(size);
    bzero(n, size);
    n->obj_id = obj_id;
    n->bound_arg = parent->bound_arg;
    n->sat_pre = parent->sat_pre;
    BOUND_ARG_SET(n, arg_id);
    return n;
}

static void tnodeDel(tnode_t *t)
{
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

static tnode_t *tnodeAddChild(trie_t *t, tnode_t *par,
                              int arg_id, obj_id_t obj_id)
{
    tnode_t *n = tnodeNew(t, par, arg_id, obj_id);
    tnodeReserveChild(par);
    par->child[par->child_size++] = n;
    return n;
}



struct bnode {
    int param_id;
    int obj_id;
    uint32_t bound_arg; /*!< Max 32 arguments allowed! */
    uint32_t sat_pre;   /*!< Max 32 preconditions */
    struct bnode **child;
    int child_size;
    int child_alloc;
};
typedef struct bnode bnode_t;

struct btrie {
    int action_id;
    int param_size;
    int pre_size;
    uint32_t bound_arg_mask;
    uint32_t sat_pre_mask;
    int **pred_to_pre;
    int *pred_to_pre_size;
    bnode_t root;
};
typedef struct btrie btrie_t;

struct ground {
    const pddl_t *pddl;
    pddl_prep_actions_t action;
    pddl_facts_t static_fact;
    pddl_facts_t fact;
    pddl_strips_ops_t op;
    btrie_t *trie;
};
typedef struct ground ground_t;

static bnode_t *bnodeNew(const bnode_t *parent, int obj_id)
{
    bnode_t *bn;

    bn = (bnode_t *)BOR_CALLOC_ARR(char, sizeof(bnode_t));
    bn->param_id = -1;
    bn->obj_id = obj_id;
    bn->bound_arg = parent->bound_arg;
    bn->bound_arg |= (1u << parent->param_id);
    bn->sat_pre = parent->sat_pre;
    return bn;
}

static void bnodeDel(bnode_t *bn)
{
    for (int i = 0; i < bn->child_size; ++i)
        bnodeDel(bn->child[i]);
    if (bn->child != NULL)
        BOR_FREE(bn->child);
    BOR_FREE(bn);
}

static bnode_t *bnodeAddChild(bnode_t *par, int obj_id)
{
    bnode_t *bn = bnodeNew(par, obj_id);
    if (par->child_size >= par->child_alloc){
        if (par->child_alloc == 0)
            par->child_alloc = 1;
        par->child_alloc *= 2;
        par->child = BOR_REALLOC_ARR(par->child, bnode_t *, par->child_alloc);
    }
    par->child[par->child_size++] = bn;
    return bn;
}

static void bnodeTraverse(bnode_t *bn,
                          void (*pre)(bnode_t *, void *),
                          void (*post)(bnode_t *, void *),
                          void *u)
{
    if (pre)
        pre(bn, u);
    for (int i = 0; i < bn->child_size; ++i)
        bnodeTraverse(bn, pre, post, u);
    if (post)
        post(bn, u);
}

static void btrieInit(btrie_t *bt, const ground_t *g, int action_id)
{
    const pddl_prep_action_t *a = g->action.action + action_id;
    const pddl_cond_atom_t *atom;

    bzero(bt, sizeof(*bt));
    bt->action_id = action_id;
    bt->param_size = a->param_size;
    bt->pre_size = a->pre.size;
    bt->root.param_id = -1;

    for (int i = 0; i < bt->param_size; ++i)
        bt->bound_arg_mask = (bt->bound_arg_mask << 1u) | 1u;
    for (int i = 0; i < a->pre.size; ++i)
        bt->sat_pre_mask = (bt->sat_pre_mask << 1u) | 1u;

    bt->pred_to_pre_size = BOR_CALLOC_ARR(int, bt->pre_size);
    bt->pred_to_pre = BOR_CALLOC_ARR(int *, bt->pre_size);
    for (int i = 0; i < a->pre.size; ++i){
        atom = PDDL_COND_CAST(a->pre.cond[i], atom);
        ++bt->pred_to_pre_size[atom->pred];
        bt->pred_to_pre[atom->pred] = BOR_REALLOC_ARR(
                                        bt->pred_to_pre[atom->pred],
                                        int, bt->pred_to_pre_size[atom->pred]);
        bt->pred_to_pre[atom->pred][bt->pred_to_pre_size[atom->pred] - 1] = i;
    }
}

static void btrieFree(btrie_t *bt)
{
    for (int i = 0; i < bt->root.child_size; ++i)
        bnodeDel(bt->root.child[i]);
    for (int i = 0; i < bt->pre_size; ++i){
        if (bt->pred_to_pre[i] != NULL)
            BOR_FREE(bt->pred_to_pre[i]);
    }
    if (bt->pred_to_pre != NULL)
        BOR_FREE(bt->pred_to_pre);
    if (bt->pred_to_pre_size != NULL)
        BOR_FREE(bt->pred_to_pre_size);
}

static bnode_t *btrieFindOrCreate(btrie_t *bt, const int *arg)
{
    bnode_t *root = &bt->root;

    // Initialization of the whole trie if necessary
    if (root->param_id < 0){
        for (int i = 0; i < bt->param_size; ++i){
            if (arg[i] >= 0){
                root->param_id = i;
                root = bnodeAddChild(root, arg[i]);
            }
        }
        return root;
    }

    // Find leaf node corresponding to the bounded arguments
    while (root->param_id >= 0){
        for (int i = 0; i < root->child_size; ++i){
            if (root->child[i]->obj_id == arg[root->param_id]){
                root = root->child[i];
                break;
            }
        }
    }

    // Check if need to create deeper trie to catch all set value
    for (int i = 0; i < bt->param_size; ++i){
        if (arg[i] >= 0 && !(root->bound_arg & (1u << i))){
            root->param_id = i;
            root = bnodeAddChild(root, arg[i]);
        }
    }

    return root;
}

#endif

/*






static void bnodeInit(bnode_t *bn)
{
    bzero(bn, sizeof(*bn));
}
static bnode_t *bnodeNew(void)
{
    bnode_t *bn = BOR_ALLOC(bnode_t);
    bnodeInit(bn);
    return bn;
}

static void bnodeFree(bnode_t *bn)
{
}
static void bnodeDel(bnode_t *bn)
{
    BOR_FREE(bn);
}

static int bnodeActionId(const bnode_t *bn)
{
    if (bn->parent == NULL){
        btree_t *b = bor_container_of(bn, btree_t, root);
        return b->action_id;
    }else{
        return bnodeActionId(bn->parent);
    }
}

static void _bnodeSetArgs(const bnode_t *bn, int *arg)
{
    if (bn->parent != NULL){
        arg[bn->param_id] = bn->obj_id;
        _bnodeSetArgs(bn, arg);
    }
}
static void bnodeSetArgs(const bnode_t *bn, int *arg, int arg_size)
{
    for (int i = 0; i < arg_size; ++i)
        arg[i] = -1;
    _bnodeSetArgs(bn, arg);
}

static int _bnodeHash(const bnode_t *bn, int **buf, int level)
{
    if (bn->parent == NULL){
        btree_t *btree = bor_container_of(bn, btree_t, root);
        *buf = BOR_ALLOC_ARR(int, 2 * level + 1);
        (*buf)[2 * level] = btree->action_id;
        return 2 * level + 1;

    }else{
        int ret = _bnodeHash(bn->parent, buf, level + 1);
        (*buf)[2 * level] = bn->param_id;
        (*buf)[2 * level + 1] = bn->obj_id;
        return ret;
    }
}

static int hashBufCmp(const void *a, const void *b, void *_)
{
    return memcmp(a, b, sizeof(int) * 2);
}

static uint64_t bnodeHash(const bnode_t *bn)
{
    int *buf;
    int size = _bnodeHash(bn, &buf, 0);
    borSort(buf, size / 2, sizeof(int) * 2, hashBufCmp, NULL);
    uint64_t h = borFastHash_64(buf, sizeof(int) * size, 7283);
    BOR_FREE(buf);
    return h;
}

static void _bnodeEqCheck(const bnode_t *bn, int *arg)
{
    if (bn->parent != NULL){
        arg[bn->param_id] -= bn->obj_id;
        _bnodeEqCheck(bn->parent, arg);
    }
}

static int bnodeEq(const ground_t *g, const bnode_t *b1, const bnode_t *b2)
{
    int aid1 = bnodeActionId(b1);
    int aid2 = bnodeActionId(b2);
    if (aid1 != aid2)
        return 0;
    int arg_size = g->action.action[aid1].param_size;
    int arg[arg_size];
    bnodeSetArgs(b1, arg, arg_size);
    _bnodeEqCheck(b2, arg);
    for (int i = 0; i < arg_size; ++i){
        if (arg[i] != -1)
            return 0;
    }
    return 1;
}


static bor_htable_key_t bnodeHashCB(const bor_list_t *key, void *ud)
{
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
        for (int i = 0; i < pddl->obj.size; ++i){
            PDDL_FACT_FOR_GROUND2(eq_fact, 2);
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
    pddlGroundActionsInit(pddl, &g->action);
    pddlFactsInit(&g->static_fact);
    pddlFactsInit(&g->fact);
    pddlStripsOpsInit(&g->op);

    groundInitStaticFact(g, pddl);
    groundInitFact(g, pddl);
}

static void groundFree(ground_t *g)
{
    pddlGroundActionsFree(&g->action);
    pddlFactsFree(&g->static_fact);
    pddlFactsFree(&g->fact);
    pddlStripsOpsFree(&g->op);
}

void _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl)
{
    ground_t g;
    groundInit(&g, pddl);
    groundFree(&g);
}
*/
