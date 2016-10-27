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

#include "pddl/cond.h"
#include "err.h"

typedef void (*pddl_cond_method_del_fn)(pddl_cond_t *);
typedef pddl_cond_t *(*pddl_cond_method_clone_fn)(const pddl_cond_t *);
typedef void (*pddl_cond_method_traverse_fn)(pddl_cond_t *, int post,
                        void (*cb)(pddl_cond_t *, void *), void *);

struct pddl_cond_cls {
    pddl_cond_method_del_fn del;
    pddl_cond_method_clone_fn clone;
    pddl_cond_method_traverse_fn traverse;
};
typedef struct pddl_cond_cls pddl_cond_cls_t;

#define METHOD(X, NAME) ((pddl_cond_method_##NAME##_fn)(X))
#define MCLS(NAME) \
    { .del = METHOD(cond##NAME##Del, del), \
      .clone = METHOD(cond##NAME##Clone, clone), \
      .traverse = METHOD(cond##NAME##Traverse, traverse), \
    }


struct parse_ctx {
    const pddl_types_t *types;
    const pddl_objs_t *objs;
    const pddl_type_obj_t *type_obj;
    const pddl_preds_t *preds;
    const pddl_preds_t *funcs;
    const pddl_params_t *params;
    const char *err;
};
typedef struct parse_ctx parse_ctx_t;

#define OBJ(C, T) \
    bor_container_of((C), pddl_cond_##T##_t, cls)


static void condPartDel(pddl_cond_part_t *);
static pddl_cond_part_t *condPartClone(const pddl_cond_part_t *p);
static void condPartTraverse(pddl_cond_part_t *, int post,
                             void (*cb)(pddl_cond_t *, void *), void *);

static void condQuantDel(pddl_cond_quant_t *);
static pddl_cond_quant_t *condQuantClone(const pddl_cond_quant_t *q);
static void condQuantTraverse(pddl_cond_quant_t *, int post,
                              void (*cb)(pddl_cond_t *, void *), void *);

static void condWhenDel(pddl_cond_when_t *);
static pddl_cond_when_t *condWhenClone(const pddl_cond_when_t *w);
static void condWhenTraverse(pddl_cond_when_t *, int post,
                             void (*cb)(pddl_cond_t *, void *), void *);

static void condAtomDel(pddl_cond_atom_t *);
static pddl_cond_atom_t *condAtomClone(const pddl_cond_atom_t *a);
static void condAtomTraverse(pddl_cond_atom_t *, int post,
                             void (*cb)(pddl_cond_t *, void *), void *);

static void condAssignDel(pddl_cond_assign_t *);
static pddl_cond_assign_t *condAssignClone(const pddl_cond_assign_t *a);
static void condAssignTraverse(pddl_cond_assign_t *, int post,
                               void (*cb)(pddl_cond_t *, void *), void *);

static pddl_cond_cls_t cond_cls[7] = {
    MCLS(Part),  // PDDL_COND_AND
    MCLS(Part),  // PDDL_COND_OR
    MCLS(Quant), // PDDL_COND_FORALL
    MCLS(Quant), // PDDL_COND_EXIST
    MCLS(When),  // PDDL_COND_WHEN
    MCLS(Atom),  // PDDL_COND_ATOM
    MCLS(Assign) // PDDL_COND_ASSIGN
};

static pddl_cond_t *parse(const pddl_lisp_node_t *root,
                          const parse_ctx_t *ctx,
                          int negated);

#define condNew(CTYPE, TYPE) \
    (CTYPE *)_condNew(sizeof(CTYPE), TYPE)

static pddl_cond_t *_condNew(int size, unsigned type)
{
    pddl_cond_t *c;

    c = BOR_MALLOC(size);
    bzero(c, size);
    c->type = type;
    borListInit(&c->conn);
    return c;
}


/*** PART ***/
static pddl_cond_part_t *condPartNew(int type)
{
    pddl_cond_part_t *p;
    p = condNew(pddl_cond_part_t, type);
    borListInit(&p->part);
    return p;
}

static void condPartDel(pddl_cond_part_t *p)
{
    bor_list_t *item, *tmp;
    pddl_cond_t *cond;

    BOR_LIST_FOR_EACH_SAFE(&p->part, item, tmp){
        cond = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        pddlCondDel(cond);
    }

    BOR_FREE(p);
}

static pddl_cond_part_t *condPartClone(const pddl_cond_part_t *p)
{
    pddl_cond_part_t *n;
    pddl_cond_t *c, *nc;
    bor_list_t *item;

    n = condPartNew(p->cls.type);
    BOR_LIST_FOR_EACH(&p->part, item){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        nc = pddlCondClone(c);
        borListAppend(&n->part, &nc->conn);
    }
    return n;
}

static void condPartTraverse(pddl_cond_part_t *p, int post,
                             void (*cb)(pddl_cond_t *, void *), void *u)
{
    pddl_cond_t *c;
    bor_list_t *item, *tmp;

    BOR_LIST_FOR_EACH_SAFE(&p->part, item, tmp){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        pddlCondTraverse(c, post, cb, u);
    }
}

/** Moves all parts of src to dst */
static void condPartStealPart(pddl_cond_part_t *dst,
                              pddl_cond_part_t *src)
{
    bor_list_t *item;

    while (!borListEmpty(&src->part)){
        item = borListNext(&src->part);
        borListDel(item);
        borListAppend(&dst->part, item);
    }
}


/*** QUANT ***/
static pddl_cond_quant_t *condQuantNew(int type)
{
    return condNew(pddl_cond_quant_t, type);
}

static void condQuantDel(pddl_cond_quant_t *q)
{
    pddlParamsFree(&q->param);
    if (q->cond != NULL)
        pddlCondDel(q->cond);
    BOR_FREE(q);
}

static pddl_cond_quant_t *condQuantClone(const pddl_cond_quant_t *q)
{
    pddl_cond_quant_t *n;

    n = condQuantNew(q->cls.type);
    pddlParamsCopy(&n->param, &q->param);
    n->cond = pddlCondClone(q->cond);
    return n;
}

static void condQuantTraverse(pddl_cond_quant_t *q, int post,
                              void (*cb)(pddl_cond_t *, void *), void *u)
{
    pddlCondTraverse(q->cond, post, cb, u);
}


/*** WHEN ***/
static pddl_cond_when_t *condWhenNew(void)
{
    return condNew(pddl_cond_when_t, PDDL_COND_WHEN);
}

static void condWhenDel(pddl_cond_when_t *w)
{
    if (w->pre)
        pddlCondDel(w->pre);
    if (w->eff)
        pddlCondDel(w->eff);
    BOR_FREE(w);
}

static pddl_cond_when_t *condWhenClone(const pddl_cond_when_t *w)
{
    pddl_cond_when_t *n;

    n = condWhenNew();
    n->pre = pddlCondClone(w->pre);
    n->eff = pddlCondClone(w->eff);
    return n;
}

static void condWhenTraverse(pddl_cond_when_t *w, int post,
                             void (*cb)(pddl_cond_t *, void *), void *u)
{
    pddlCondTraverse(w->pre, post, cb, u);
    pddlCondTraverse(w->eff, post, cb, u);
}


/*** ATOM ***/
static pddl_cond_atom_t *condAtomNew(void)
{
    return condNew(pddl_cond_atom_t, PDDL_COND_ATOM);
}

static void condAtomDel(pddl_cond_atom_t *a)
{
    if (a->arg != NULL)
        BOR_FREE(a->arg);
    BOR_FREE(a);
}

static pddl_cond_atom_t *condAtomClone(const pddl_cond_atom_t *a)
{
    pddl_cond_atom_t *n;

    n = condAtomNew();
    n->pred = a->pred;
    n->arg_size = a->arg_size;
    n->arg = BOR_ALLOC_ARR(pddl_cond_atom_arg_t, n->arg_size);
    memcpy(n->arg, a->arg, sizeof(pddl_cond_atom_arg_t) * n->arg_size);
    n->neg = a->neg;

    return n;
}

static void condAtomTraverse(pddl_cond_atom_t *a, int post,
                             void (*cb)(pddl_cond_t *, void *), void *u)
{
}


/*** ASSIGN ***/
static pddl_cond_assign_t *condAssignNew(void)
{
    return condNew(pddl_cond_assign_t, PDDL_COND_ASSIGN);
}

static void condAssignDel(pddl_cond_assign_t *assign)
{
    if (assign->fvalue)
        condAtomDel(assign->fvalue);
    BOR_FREE(assign);
}

static pddl_cond_assign_t *condAssignClone(const pddl_cond_assign_t *a)
{
    pddl_cond_assign_t *n;
    n = condAssignNew();
    n->value = a->value;
    if (a->fvalue)
        n->fvalue = condAtomClone(a->fvalue);
    return n;
}

static void condAssignTraverse(pddl_cond_assign_t *a, int post,
                               void (*cb)(pddl_cond_t *, void *), void *u)
{
    if (a->fvalue)
        pddlCondTraverse(&a->fvalue->cls, post, cb, u);
}




void pddlCondDel(pddl_cond_t *cond)
{
    cond_cls[cond->type].del(cond);
}

pddl_cond_t *pddlCondClone(const pddl_cond_t *cond)
{
    return cond_cls[cond->type].clone(cond);
}


void pddlCondTraverse(pddl_cond_t *c, int post,
                      void (*cb)(pddl_cond_t *, void *), void *u)
{
    if (!post)
        cb(c, u);

    cond_cls[c->type].traverse(c, post, cb, u);

    if (post)
        cb(c, u);
}

/*** PARSE ***/
static int parseAtomArg(pddl_cond_atom_arg_t *arg,
                        const pddl_lisp_node_t *root,
                        const parse_ctx_t *ctx)
{
    int v;

    if (root->value[0] == '?'){
        v = pddlParamsGetId(ctx->params, root->value);
        if (v < 0){
            ERRN(root, "Invalid paramenter `%s' :: %s", root->value, ctx->err);
            return -1;
        }
        arg->param = v;
        arg->obj = -1;

    }else{
        v = pddlObjsGet(ctx->objs, root->value);
        if (v < 0){
            ERRN(root, "Unkown constant `%s' :: %s", root->value, ctx->err);
            return -1;
        }
        arg->param = -1;
        arg->obj = v;
    }

    return 0;
}

static pddl_cond_t *parseAtom(const pddl_lisp_node_t *root,
                              const parse_ctx_t *ctx,
                              int negated)
{
    pddl_cond_atom_t *atom;
    const char *name;
    int pred, i;

    // Get predicate name
    name = pddlLispNodeHead(root);
    if (name == NULL){
        ERRN(root, "Invalid atom: missing head of expression :: %s", ctx->err);
        return NULL;
    }

    // And resolve it against known predicates
    pred = pddlPredsGet(ctx->preds, name);
    if (pred == -1){
        ERRN(root, "Unkown predicate `%s' :: %s", name, ctx->err);
        return NULL;
    }

    // Check that all children are terminals
    for (i = 1; i < root->child_size; ++i){
        if (root->child[i].value == NULL){
            ERRN(root, "Invalid instantiation of atom `%s' :: %s",
                 name, ctx->err);
            return NULL;
        }
    }

    atom = condAtomNew();
    atom->pred = pred;
    atom->arg_size = root->child_size - 1;
    atom->arg = BOR_ALLOC_ARR(pddl_cond_atom_arg_t, atom->arg_size);
    for (i = 0; i < atom->arg_size; ++i){
        if (parseAtomArg(atom->arg + i, root->child + i + 1, ctx) != 0){
            condAtomDel(atom);
            return NULL;
        }
    }
    atom->neg = negated;

    return &atom->cls;
}

static pddl_cond_t *parseAssign(const pddl_lisp_node_t *root,
                                const parse_ctx_t *ctx,
                                int negated)
{
    pddl_cond_assign_t *assign;
    pddl_cond_atom_t *fvalue;
    parse_ctx_t sub_ctx;

    if (root->child_size != 3
            || root->child[1].value != NULL
            || root->child[1].child_size != 1
            || root->child[1].child[0].value == NULL
            || strcmp(root->child[1].child[0].value, "total-cost") != 0){
        ERRN(root, "Only (increase (total-cost) int-value) is supported"
                   " :: %s", ctx->err);
        return NULL;
    }

    if (root->child[2].value != NULL){
        assign = condAssignNew();
        assign->value = atoi(root->child[2].value);

    }else{
        sub_ctx = *ctx;
        sub_ctx.preds = sub_ctx.funcs;
        fvalue = (pddl_cond_atom_t *)parseAtom(root->child + 2, &sub_ctx, negated);
        if (fvalue == NULL)
            return NULL;
        assign = condAssignNew();
        assign->fvalue = fvalue;
    }

    return &assign->cls;
}

static pddl_cond_t *parsePart(int part_type,
                              const pddl_lisp_node_t *root,
                              const parse_ctx_t *ctx,
                              int negated)
{
    pddl_cond_part_t *part;
    pddl_cond_t *cond;
    int i;

    part = condPartNew(part_type);
    for (i = 1; i < root->child_size; ++i){
        cond = parse(root->child + i, ctx, negated);
        if (cond == NULL){
            condPartDel(part);
            return NULL;
        }
        borListAppend(&part->part, &cond->conn);
    }

    return &part->cls;
}

static pddl_cond_t *parseImply(const pddl_lisp_node_t *left,
                               const pddl_lisp_node_t *right,
                               const parse_ctx_t *ctx,
                               int negated)
{
    pddl_cond_part_t *part;
    pddl_cond_t *cleft = NULL, *cright = NULL;

    if (negated){
        if ((cleft = parse(left, ctx, 0)) == NULL)
            return NULL;

        if ((cright = parse(right, ctx, 1)) == NULL){
            pddlCondDel(cleft);
            return NULL;
        }

        part = condPartNew(PDDL_COND_AND);

    }else{
        if ((cleft = parse(left, ctx, 1)) == NULL)
            return NULL;

        if ((cright = parse(right, ctx, 0)) == NULL){
            pddlCondDel(cleft);
            return NULL;
        }

        part = condPartNew(PDDL_COND_OR);
    }

    borListAppend(&part->part, &cleft->conn);
    borListAppend(&part->part, &cright->conn);

    return &part->cls;
}

static int parseQuantParams(pddl_params_t *params,
                            const pddl_lisp_node_t *root,
                            const parse_ctx_t *ctx)
{
    pddl_param_t *param;
    int i, j, use;

    pddlParamsInit(params);

    // Parse all parameters of the quantifier
    if (pddlParamsParse(params, root, ctx->types) != 0){
        pddlParamsFree(params);
        return -1;
    }

    // And also add all global parameters that are not shadowed
    for (i = 0; i < ctx->params->size; ++i){
        use = 1;
        for (j = 0; j < params->size; ++j){
            if (strcmp(params->param[j].name, ctx->params->param[i].name) == 0){
                use = 0;
                break;
            }
        }

        if (use){
            param = pddlParamsAdd(params);
            pddlParamCopy(param, ctx->params->param + i);
            param->inherit = i;
        }
    }

    return 0;
}

static pddl_cond_t *parseQuant(int quant_type,
                               const pddl_lisp_node_t *root,
                               const parse_ctx_t *ctx,
                               int negated)
{
    pddl_cond_quant_t *q;
    pddl_params_t params;
    pddl_cond_t *cond;
    parse_ctx_t sub_ctx;

    if (root->child_size != 3
            || root->child[1].value != NULL
            || root->child[2].value != NULL){
        if (quant_type == PDDL_COND_FORALL){
            ERRN(root, "Invalid (forall ...) condition :: %s", ctx->err);
        }else{
            ERRN(root, "Invalid (exists ...) condition :: %s", ctx->err);
        }
        return NULL;
    }

    if (parseQuantParams(&params, root->child + 1, ctx) != 0)
        return NULL;

    if (params.size == 0){
        pddlParamsFree(&params);
        ERRN(root, "Missing arguments in quantifier :: %s", ctx->err);
        return NULL;
    }

    sub_ctx = *ctx;
    sub_ctx.params = &params;
    cond = parse(root->child + 2, &sub_ctx, negated);
    if (cond == NULL){
        pddlParamsFree(&params);
        return NULL;
    }

    q = condQuantNew(quant_type);
    q->param = params;
    q->cond = cond;

    return &q->cls;
}

static pddl_cond_t *parseWhen(const pddl_lisp_node_t *root,
                              const parse_ctx_t *ctx)
{
    pddl_cond_when_t *w;
    pddl_cond_t *pre, *eff;

    if (root->child_size != 3
            || root->child[1].value != NULL
            || root->child[2].value != NULL){
        ERRN(root, "Invalid (when ...) condition :: %s", ctx->err);
        return NULL;
    }

    if ((pre = parse(root->child + 1, ctx, 0)) == NULL)
        return NULL;

    if ((eff = parse(root->child + 2, ctx, 0)) == NULL){
        pddlCondDel(pre);
        return NULL;
    }

    w = condWhenNew();
    w->pre = pre;
    w->eff = eff;
    return &w->cls;
}

static pddl_cond_t *parse(const pddl_lisp_node_t *root,
                          const parse_ctx_t *ctx,
                          int negated)
{
    int kw;

    kw = pddlLispNodeHeadKw(root);

    if (kw == PDDL_KW_NOT){
        if (root->child_size != 2){
            ERRN(root, "Invalid (not ...) :: %s", ctx->err);
            return NULL;
        }

        return parse(root->child + 1, ctx, !negated);

    }else if (kw == PDDL_KW_AND){
        if (root->child_size <= 1){
            ERRN(root, "Invalid (and ...) :: %s", ctx->err);
            return NULL;
        }

        if (negated){
            return parsePart(PDDL_COND_OR, root, ctx, negated);
        }else{
            return parsePart(PDDL_COND_AND, root, ctx, negated);
        }

    }else if (kw == PDDL_KW_OR){
        if (root->child_size <= 1){
            ERRN(root, "Invalid (or ...) :: %s", ctx->err);
            return NULL;
        }

        if (negated){
            return parsePart(PDDL_COND_AND, root, ctx, negated);
        }else{
            return parsePart(PDDL_COND_OR, root, ctx, negated);
        }

    }else if (kw == PDDL_KW_IMPLY){
        if (root->child_size != 3){
            ERRN(root, "Invalid (imply ...) :: %s", ctx->err);
            return NULL;
        }

        return parseImply(root->child + 1, root->child + 2, ctx, negated);

    }else if (kw == PDDL_KW_FORALL){
        // TODO: :conditional-effects || :universal-preconditions
        if (negated){
            return parseQuant(PDDL_COND_EXIST, root, ctx, negated);
        }else{
            return parseQuant(PDDL_COND_FORALL, root, ctx, negated);
        }

    }else if (kw == PDDL_KW_EXISTS){
        // TODO: :existential-preconditions
        if (negated){
            return parseQuant(PDDL_COND_FORALL, root, ctx, negated);
        }else{
            return parseQuant(PDDL_COND_EXIST, root, ctx, negated);
        }

    }else if (kw == PDDL_KW_WHEN){
        // Conditional effect cannot be negated
        return parseWhen(root, ctx);

    }else if (kw == PDDL_KW_INCREASE){
        return parseAssign(root, ctx, negated);

    }else if (kw == -1){
        return parseAtom(root, ctx, negated);
    }

    if (root->child_size >= 1
            && root->child[0].value != NULL){
        ERRN(root, "Unexpected token `%s' :: %s",
                   root->child[0].value, ctx->err);
    }else{
        ERRN(root, "Unexpected token :: %s", ctx->err);
    }

    return NULL;
}

pddl_cond_t *pddlCondParse(const pddl_lisp_node_t *root,
                           const pddl_types_t *types,
                           const pddl_objs_t *objs,
                           const pddl_type_obj_t *type_obj,
                           const pddl_preds_t *preds,
                           const pddl_preds_t *funcs,
                           const pddl_params_t *params,
                           const char *err)
{
    parse_ctx_t ctx;

    ctx.types = types;
    ctx.objs = objs;
    ctx.type_obj = type_obj;
    ctx.preds = preds;
    ctx.funcs = funcs;
    ctx.params = params;
    ctx.err = err;

    return parse(root, &ctx, 0);
}


/*** CHECK ***/
int pddlCondCheckPre(const pddl_cond_t *cond,
                     int require,
                     int verbose)
{
    pddl_cond_part_t *p;
    pddl_cond_quant_t *q;
    pddl_cond_atom_t *atom;
    pddl_cond_t *c;
    bor_list_t *item;

    if (cond->type == PDDL_COND_AND
            || cond->type == PDDL_COND_OR){
        if (cond->type == PDDL_COND_OR
                && !(require & PDDL_REQUIRE_DISJUNCTIVE_PRE)){
            if (verbose){
                ERR2("(forall ...) can be used only with"
                     " :disjunctive-preconditions");
            }
            return -1;
        }

        p = OBJ(cond, part);
        BOR_LIST_FOR_EACH(&p->part, item){
            c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            if (pddlCondCheckPre(c, require, verbose) != 0)
                return -1;
        }

        return 0;

    }else if (cond->type == PDDL_COND_FORALL){
        if (!(require & PDDL_REQUIRE_UNIVERSAL_PRE)){
            if (verbose){
                ERR2("(forall ...) can be used only with"
                     " :universal-preconditions");
            }
            return -1;
        }

        q = OBJ(cond, quant);
        return pddlCondCheckPre(q->cond, require, verbose);

    }else if (cond->type == PDDL_COND_EXIST){
        if (!(require & PDDL_REQUIRE_EXISTENTIAL_PRE)){
            if (verbose){
                ERR2("(exists ...) can be used only with"
                     " :existential-preconditions");
            }
            return -1;
        }

        q = OBJ(cond, quant);
        return pddlCondCheckPre(q->cond, require, verbose);

    }else if (cond->type == PDDL_COND_WHEN){
        if (verbose){
            ERR2("(when ...) cannot be part of preconditions");
        }
        return -1;

    }else if (cond->type == PDDL_COND_ATOM){
        atom = OBJ(cond, atom);
        if (atom->neg && !(require & PDDL_REQUIRE_NEGATIVE_PRE)){
            if (verbose){
                ERR2("For negative preconditions add :negative-preconditions");
            }
            return -1;
        }

        return 0;

    }else if (cond->type == PDDL_COND_ASSIGN){
        return 0;
    }

    return -1;
}


static int checkCEffect(const pddl_cond_t *cond, int require, int verbose);
static int checkPEffect(const pddl_cond_t *cond, int require, int verbose);
static int checkCondEffect(const pddl_cond_t *cond, int require, int verbose);

static int checkCEffect(const pddl_cond_t *cond, int require, int verbose)
{
    pddl_cond_quant_t *forall;
    pddl_cond_when_t *when;

    if (cond->type == PDDL_COND_FORALL){
        if (!(require & PDDL_REQUIRE_CONDITIONAL_EFF)){
            if (verbose){
                ERR2("(forall ...) is allowed in effects only if"
                     " :conditional-effects is specified as requirement");
            }
            return -1;
        }

        forall = OBJ(cond, quant);
        return pddlCondCheckEff(forall->cond, require, verbose);

    }else if (cond->type == PDDL_COND_WHEN){
        if (!(require & PDDL_REQUIRE_CONDITIONAL_EFF)){
            if (verbose){
                ERR2("(when ...) is allowed in effects only if"
                     " :conditional-effects is specified as requirement");
            }
            return -1;
        }

        when = OBJ(cond, when);
        if (pddlCondCheckPre(when->pre, require, verbose) != 0)
            return -1;
        return checkCondEffect(when->eff, require, verbose);

    }else{
        if (checkPEffect(cond, require, verbose) != 0){
            if (verbose){
                ERR2("A single effect has to be either literal or"
                     " conditional effect (+ universal quantifier).");
            }
            return -1;
        }
        return 0;
    }
}

static int checkPEffect(const pddl_cond_t *cond, int require, int verbose)
{
    if (cond->type == PDDL_COND_ATOM
            || cond->type == PDDL_COND_ASSIGN)
        return 0;
    return -1;
}

static int checkCondEffect(const pddl_cond_t *cond, int require, int verbose)
{
    const pddl_cond_part_t *part;
    const pddl_cond_t *sub;
    bor_list_t *item;

    if (checkPEffect(cond, require, verbose) == 0)
        return 0;

    if (cond->type == PDDL_COND_AND){
        part = OBJ(cond, part);
        BOR_LIST_FOR_EACH(&part->part, item){
            sub = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            if (checkPEffect(sub, require, verbose) != 0){
                if (verbose){
                    ERR2("Conditional effect can contain only literals and"
                         " conjuction of literals.");
                }
                return -1;
            }
        }

        return 0;
    }

    return -1;
}

int pddlCondCheckEff(const pddl_cond_t *cond,
                     int require,
                     int verbose)
{
    const pddl_cond_part_t *and;
    const pddl_cond_t *sub;
    bor_list_t *item;

    if (cond->type == PDDL_COND_AND){
        and = OBJ(cond, part);
        BOR_LIST_FOR_EACH(&and->part, item){
            sub = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            if (checkCEffect(sub, require, verbose) != 0)
                return -1;
        }

        return 0;

    }else{
        return checkCEffect(cond, require, verbose);
    }
}



int pddlCondFlatten(pddl_cond_t *cond)
{
    pddl_cond_part_t *part, *p;
    pddl_cond_quant_t *q;
    pddl_cond_when_t *w;
    pddl_cond_t *c;
    bor_list_t *item, *tmp;

    if (cond->type == PDDL_COND_AND
            || cond->type == PDDL_COND_OR){
        part = OBJ(cond, part);
        BOR_LIST_FOR_EACH_SAFE(&part->part, item, tmp){
            c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            if (pddlCondFlatten(c) != 0)
                return -1;

            if (c->type == cond->type){
                // Flatten con/disjunctions
                p = OBJ(c, part);
                condPartStealPart(part, p);

                borListDel(item);
                pddlCondDel(c);

            }else if (c->type == PDDL_COND_AND
                        || c->type == PDDL_COND_OR){
                p = OBJ(c, part);

                // If con/disjunction has only one atom, the parent one can
                // safely take that atom
                if (borListPrev(&p->part) == borListNext(&p->part)){
                    condPartStealPart(part, p);
                    borListDel(item);
                    pddlCondDel(c);
                }
            }
        }

        // If disjunction has only one atom change it do conjuction.
        if (borListPrev(&part->part) == borListNext(&part->part))
            part->cls.type = PDDL_COND_AND;

        return 0;

    }else if (cond->type == PDDL_COND_FORALL
                || cond->type == PDDL_COND_EXIST){
        q = OBJ(cond, quant);
        return pddlCondFlatten(q->cond);

    }else if (cond->type == PDDL_COND_WHEN){
        w = OBJ(cond, when);
        if (pddlCondFlatten(w->pre) == 0
                && pddlCondFlatten(w->eff) == 0)
            return 0;
        return -1;

    }else if (cond->type == PDDL_COND_ATOM
                || cond->type == PDDL_COND_ASSIGN){
        return 0;

    }else{
        fprintf(stderr, "Fatal Error: Unkown cond type!\n");
        exit(-1);
    }
}


void instantiateForallSetParam(pddl_cond_t *cond,
                               int param_id, int obj_id)
{
    // TODO
}

pddl_cond_t *instantiateForallCond(pddl_cond_t *cond,
                                   int param_id, int obj_id)
{
    pddl_cond_t *n;

    n = pddlCondClone(cond);
    return n;
}

pddl_cond_t *instantiateForall(pddl_cond_quant_t *forall,
                               const pddl_type_obj_t *type_obj,
                               const pddl_params_t *params)
{
    pddl_cond_part_t *top;
    const pddl_param_t *param;
    int i;

    for (i = 0; i < forall->param.size; ++i){
        param = forall->param.param + i;
        if (param->inherit == -1){
            fprintf(stderr, "XXX: %d\n", i);
        }
    }

    fprintf(stderr, "YYY\n");
    return &forall->cls;
}

pddl_cond_t *instantiateForallPart(pddl_cond_part_t *p,
                                   const pddl_type_obj_t *type_obj,
                                   const pddl_params_t *params)
{
    pddl_cond_t *c;
    bor_list_t *item, *last;

    if (borListEmpty(&p->part))
        return &p->cls;

    last = borListPrev(&p->part);
    do {
        item = borListNext(&p->part);
        borListDel(item);
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        c = pddlCondInstantiateForall(c, type_obj, params);
        borListAppend(&p->part, &c->conn);
    } while (item != last);

    return &p->cls;
}

pddl_cond_t *pddlCondInstantiateForall(pddl_cond_t *cond,
                                       const pddl_type_obj_t *type_obj,
                                       const pddl_params_t *params)
{
    pddl_cond_quant_t *q;
    pddl_cond_when_t *w;

    if (cond->type == PDDL_COND_OR
            || cond->type == PDDL_COND_AND){
        return instantiateForallPart(OBJ(cond, part), type_obj, params);

    }else if (cond->type == PDDL_COND_FORALL){
        q = OBJ(cond, quant);
        return instantiateForall(q, type_obj, params);

    }else if (cond->type == PDDL_COND_EXIST){
        q = OBJ(cond, quant);
        q->cond = pddlCondInstantiateForall(q->cond, type_obj, &q->param);
        return cond;

    }else if (cond->type == PDDL_COND_WHEN){
        w = OBJ(cond, when);
        w->pre = pddlCondInstantiateForall(w->pre, type_obj, params);
        w->eff = pddlCondInstantiateForall(w->eff, type_obj, params);
        return cond;

    }else if (cond->type == PDDL_COND_ATOM
                || cond->type == PDDL_COND_ASSIGN){
        return cond;

    }else{
        fprintf(stderr, "Fatal Error: Unkown type of condition!\n");
        exit(-1);
    }
}

/*** PRINT ***/
static void condPartPrint(const pddl_cond_part_t *cond,
                          const char *name,
                          const pddl_objs_t *objs,
                          const pddl_preds_t *preds,
                          const pddl_preds_t *funcs,
                          const pddl_params_t *params,
                          FILE *fout)
{
    bor_list_t *item;
    const pddl_cond_t *child;

    fprintf(fout, "(%s", name);
    BOR_LIST_FOR_EACH(&cond->part, item){
        child = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        fprintf(fout, " ");
        pddlCondPrint(child, objs, preds, funcs, params, fout);
    }
    fprintf(fout, ")");
}

static void condQuantPrint(const pddl_cond_quant_t *q,
                           const char *name,
                           const pddl_objs_t *objs,
                           const pddl_preds_t *preds,
                           const pddl_preds_t *funcs,
                           const pddl_params_t *params,
                           FILE *fout)
{
    fprintf(fout, "(%s", name);

    fprintf(fout, " (");
    pddlParamsPrint(&q->param, fout);
    fprintf(fout, ") ");

    pddlCondPrint(q->cond, objs, preds, funcs, &q->param, fout);

    fprintf(fout, ")");
}

static void condWhenPrint(const pddl_cond_when_t *w,
                           const pddl_objs_t *objs,
                           const pddl_preds_t *preds,
                           const pddl_preds_t *funcs,
                           const pddl_params_t *params,
                           FILE *fout)
{
    fprintf(fout, "(when ");
    pddlCondPrint(w->pre, objs, preds, funcs, params, fout);
    fprintf(fout, " ");
    pddlCondPrint(w->eff, objs, preds, funcs, params, fout);
    fprintf(fout, ")");
}

static void condAtomPrint(const pddl_cond_atom_t *atom,
                          const pddl_objs_t *objs,
                          const pddl_preds_t *preds,
                          const pddl_params_t *params,
                          FILE *fout)
{
    int i;

    fprintf(fout, "(");
    if (atom->neg)
        fprintf(fout, "N:");
    fprintf(fout, "%s", preds->pred[atom->pred].name);

    for (i = 0; i < atom->arg_size; ++i){
        fprintf(fout, " ");
        if (atom->arg[i].param >= 0){
            fprintf(fout, "%s", params->param[atom->arg[i].param].name);
        }else{
            fprintf(fout, "%s", objs->obj[atom->arg[i].obj].name);
        }
    }

    fprintf(fout, ")");
}

static void condAssignPrint(const pddl_cond_assign_t *assign,
                            const pddl_objs_t *objs,
                            const pddl_preds_t *funcs,
                            const pddl_params_t *params,
                            FILE *fout)
{
    fprintf(fout, "(increase (total-cost) ");
    if (assign->fvalue != NULL){
        condAtomPrint(assign->fvalue, objs, funcs, params, fout);
    }else{
        fprintf(fout, "%d", assign->value);
    }
    fprintf(fout, ")");
}

void pddlCondPrint(const pddl_cond_t *cond,
                   const pddl_objs_t *objs,
                   const pddl_preds_t *preds,
                   const pddl_preds_t *funcs,
                   const pddl_params_t *params,
                   FILE *fout)
{
    if (cond->type == PDDL_COND_AND){
        condPartPrint(OBJ(cond, part), "and", objs, preds, funcs, params, fout);

    }else if (cond->type == PDDL_COND_OR){
        condPartPrint(OBJ(cond, part), "or", objs, preds, funcs, params, fout);

    }else if (cond->type == PDDL_COND_FORALL){
        condQuantPrint(OBJ(cond, quant), "forall", objs,
                       preds, funcs, params, fout);

    }else if (cond->type == PDDL_COND_EXIST){
        condQuantPrint(OBJ(cond, quant), "exists", objs,
                       preds, funcs, params, fout);

    }else if (cond->type == PDDL_COND_WHEN){
        condWhenPrint(OBJ(cond, when), objs, preds, funcs, params, fout);

    }else if (cond->type == PDDL_COND_ATOM){
        condAtomPrint(OBJ(cond, atom), objs, preds, params, fout);

    }else if (cond->type == PDDL_COND_ASSIGN){
        condAssignPrint(OBJ(cond, assign), objs, funcs, params, fout);

    }else{
        fprintf(stderr, "Fatal Error: Unkown type!\n");
        exit(-1);
    }
}
