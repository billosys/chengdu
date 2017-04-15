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

#include "pddl/pddl.h"
#include "pddl/cond.h"
#include "err.h"

typedef void (*pddl_cond_method_del_fn)(pddl_cond_t *);
typedef pddl_cond_t *(*pddl_cond_method_clone_fn)(const pddl_cond_t *);
typedef int (*pddl_cond_method_traverse_fn)(pddl_cond_t *,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *userdata);
typedef int (*pddl_cond_method_rebuild_fn)(
                            pddl_cond_t **c,
                            int (*pre)(pddl_cond_t **, void *),
                            int (*post)(pddl_cond_t **, void *),
                            void *userdata);

struct pddl_cond_cls {
    pddl_cond_method_del_fn del;
    pddl_cond_method_clone_fn clone;
    pddl_cond_method_traverse_fn traverse;
    pddl_cond_method_rebuild_fn rebuild;
};
typedef struct pddl_cond_cls pddl_cond_cls_t;

#define METHOD(X, NAME) ((pddl_cond_method_##NAME##_fn)(X))
#define MCLS(NAME) \
    { .del = METHOD(cond##NAME##Del, del), \
      .clone = METHOD(cond##NAME##Clone, clone), \
      .traverse = METHOD(cond##NAME##Traverse, traverse), \
      .rebuild = METHOD(cond##NAME##Rebuild, rebuild), \
    }


struct parse_ctx {
    pddl_types_t *types;
    const pddl_objs_t *objs;
    const pddl_preds_t *preds;
    const pddl_preds_t *funcs;
    const pddl_params_t *params;
    const char *err;
};
typedef struct parse_ctx parse_ctx_t;

#define OBJ(C, T) PDDL_COND_CAST(C, T)

static void condPartDel(pddl_cond_part_t *);
static pddl_cond_part_t *condPartClone(const pddl_cond_part_t *p);
static void condPartAdd(pddl_cond_part_t *p, pddl_cond_t *add);
static int condPartTraverse(pddl_cond_part_t *,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *userdata);
static int condPartRebuild(pddl_cond_part_t **p,
                           int (*pre)(pddl_cond_t **, void *),
                           int (*post)(pddl_cond_t **, void *),
                           void *userdata);

static void condQuantDel(pddl_cond_quant_t *);
static pddl_cond_quant_t *condQuantClone(const pddl_cond_quant_t *q);
static int condQuantTraverse(pddl_cond_quant_t *,
                             int (*pre)(pddl_cond_t *, void *),
                             int (*post)(pddl_cond_t *, void *),
                             void *userdata);
static int condQuantRebuild(pddl_cond_quant_t **q,
                            int (*pre)(pddl_cond_t **, void *),
                            int (*post)(pddl_cond_t **, void *),
                            void *userdata);

static void condWhenDel(pddl_cond_when_t *);
static pddl_cond_when_t *condWhenClone(const pddl_cond_when_t *w);
static int condWhenTraverse(pddl_cond_when_t *w,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *userdata);
static int condWhenRebuild(pddl_cond_when_t **w,
                            int (*pre)(pddl_cond_t **, void *),
                            int (*post)(pddl_cond_t **, void *),
                            void *userdata);

static void condAtomDel(pddl_cond_atom_t *);
static pddl_cond_atom_t *condAtomClone(const pddl_cond_atom_t *a);
static int condAtomTraverse(pddl_cond_atom_t *,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *userdata);
static int condAtomRebuild(pddl_cond_atom_t **a,
                            int (*pre)(pddl_cond_t **, void *),
                            int (*post)(pddl_cond_t **, void *),
                            void *userdata);

static void condAssignDel(pddl_cond_assign_t *);
static pddl_cond_assign_t *condAssignClone(const pddl_cond_assign_t *a);
static int condAssignTraverse(pddl_cond_assign_t *,
                              int (*pre)(pddl_cond_t *, void *),
                              int (*post)(pddl_cond_t *, void *),
                              void *userdata);
static int condAssignRebuild(pddl_cond_assign_t **a,
                            int (*pre)(pddl_cond_t **, void *),
                            int (*post)(pddl_cond_t **, void *),
                            void *userdata);

static void condBoolDel(pddl_cond_bool_t *);
static pddl_cond_bool_t *condBoolClone(const pddl_cond_bool_t *a);
static int condBoolTraverse(pddl_cond_bool_t *,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *userdata);
static int condBoolRebuild(pddl_cond_bool_t **a,
                            int (*pre)(pddl_cond_t **, void *),
                            int (*post)(pddl_cond_t **, void *),
                            void *userdata);


static pddl_cond_cls_t cond_cls[8] = {
    MCLS(Part),   // PDDL_COND_AND
    MCLS(Part),   // PDDL_COND_OR
    MCLS(Quant),  // PDDL_COND_FORALL
    MCLS(Quant),  // PDDL_COND_EXIST
    MCLS(When),   // PDDL_COND_WHEN
    MCLS(Atom),   // PDDL_COND_ATOM
    MCLS(Assign), // PDDL_COND_ASSIGN
    MCLS(Bool),   // PDDL_COND_BOOL
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

static void condPartAdd(pddl_cond_part_t *p, pddl_cond_t *add)
{
    borListInit(&add->conn);
    borListAppend(&p->part, &add->conn);
}

static int condPartTraverse(pddl_cond_part_t *p,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *u)
{
    pddl_cond_t *c;
    bor_list_t *item, *tmp;
    int ret = 0;

    BOR_LIST_FOR_EACH_SAFE(&p->part, item, tmp){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        ret |= pddlCondTraverse(c, pre, post, u);
    }

    return ret;
}

static int condPartRebuild(pddl_cond_part_t **p,
                           int (*pre)(pddl_cond_t **, void *),
                           int (*post)(pddl_cond_t **, void *),
                           void *u)
{
    pddl_cond_t *c;
    bor_list_t *item, *last;
    int ret = 0;

    if (borListEmpty(&(*p)->part))
        return 0;

    last = borListPrev(&(*p)->part);
    do {
        item = borListNext(&(*p)->part);
        borListDel(item);
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        ret |= pddlCondRebuild(&c, pre, post, u);
        borListAppend(&(*p)->part, &c->conn);
    } while (item != last);

    return ret;
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

static int condQuantTraverse(pddl_cond_quant_t *q,
                             int (*pre)(pddl_cond_t *, void *),
                             int (*post)(pddl_cond_t *, void *),
                             void *u)
{
    if (q->cond)
        return pddlCondTraverse(q->cond, pre, post, u);
    return 0;
}

static int condQuantRebuild(pddl_cond_quant_t **q,
                            int (*pre)(pddl_cond_t **, void *),
                            int (*post)(pddl_cond_t **, void *),
                            void *userdata)
{
    if ((*q)->cond)
        return pddlCondRebuild(&(*q)->cond, pre, post, userdata);
    return 0;
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
    if (w->pre)
        n->pre = pddlCondClone(w->pre);
    if (w->eff)
        n->eff = pddlCondClone(w->eff);
    return n;
}

static int condWhenTraverse(pddl_cond_when_t *w,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *u)
{
    int ret;

    ret = pddlCondTraverse(w->pre, pre, post, u);
    ret |= pddlCondTraverse(w->eff, pre, post, u);
    return ret;
}

static int condWhenRebuild(pddl_cond_when_t **w,
                           int (*pre)(pddl_cond_t **, void *),
                           int (*post)(pddl_cond_t **, void *),
                           void *u)
{
    int ret = 0;

    if ((*w)->pre)
        ret |= pddlCondRebuild(&(*w)->pre, pre, post, u);
    if ((*w)->eff)
        ret |= pddlCondRebuild(&(*w)->eff, pre, post, u);
    return ret;
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

static int condAtomTraverse(pddl_cond_atom_t *a,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *u)
{
    return 0;
}

static int condAtomRebuild(pddl_cond_atom_t **a,
                           int (*pre)(pddl_cond_t **, void *),
                           int (*post)(pddl_cond_t **, void *),
                           void *u)
{
    return 0;
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

static int condAssignTraverse(pddl_cond_assign_t *a,
                              int (*pre)(pddl_cond_t *, void *),
                              int (*post)(pddl_cond_t *, void *),
                              void *u)
{
    return 0;
}

static int condAssignRebuild(pddl_cond_assign_t **a,
                             int (*pre)(pddl_cond_t **, void *),
                             int (*post)(pddl_cond_t **, void *),
                             void *userdata)
{
    return 0;
}


/*** BOOL ***/
static pddl_cond_bool_t *condBoolNew(int val)
{
    pddl_cond_bool_t *b;
    b = condNew(pddl_cond_bool_t, PDDL_COND_BOOL);
    b->val = val;
    return b;
}

static void condBoolDel(pddl_cond_bool_t *a)
{
    BOR_FREE(a);
}

static pddl_cond_bool_t *condBoolClone(const pddl_cond_bool_t *a)
{
    return condBoolNew(a->val);
}

static int condBoolTraverse(pddl_cond_bool_t *a,
                            int (*pre)(pddl_cond_t *, void *),
                            int (*post)(pddl_cond_t *, void *),
                            void *u)
{
    return 0;
}

static int condBoolRebuild(pddl_cond_bool_t **a,
                           int (*pre)(pddl_cond_t **, void *),
                           int (*post)(pddl_cond_t **, void *),
                           void *userdata)
{
    return 0;
}





void pddlCondDel(pddl_cond_t *cond)
{
    cond_cls[cond->type].del(cond);
}

pddl_cond_t *pddlCondClone(const pddl_cond_t *cond)
{
    return cond_cls[cond->type].clone(cond);
}


int pddlCondTraverse(pddl_cond_t *c,
                     int (*pre)(pddl_cond_t *, void *),
                     int (*post)(pddl_cond_t *, void *),
                     void *u)
{
    int ret = 0;

    if (pre != NULL){
        if (pre(c, u) != 0)
            return 0;
    }

    ret |= cond_cls[c->type].traverse(c, pre, post, u);

    if (post != NULL)
        ret |= post(c, u);
    return ret;
}

int pddlCondRebuild(pddl_cond_t **c,
                    int (*pre)(pddl_cond_t **, void *),
                    int (*post)(pddl_cond_t **, void *),
                    void *u)
{
    int ret = 0;

    if (pre != NULL){
        if (pre(c, u) != 0)
            return 0;
    }

    ret |= cond_cls[(*c)->type].rebuild(c, pre, post, u);

    if (post != NULL)
        ret |= post(c, u);
    return ret;
}

/*** PARSE ***/
static int parseAtomArg(pddl_cond_atom_arg_t *arg,
                        const pddl_lisp_node_t *root,
                        const parse_ctx_t *ctx)
{
    int v;

    if (root->value[0] == '?'){
        if (ctx->params == NULL){
            ERRN(root, "Unexpected parameter `%s' :: %s", root->value,
                 ctx->err);
            return -1;
        }

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
    for (i = 0; ctx->params != NULL && i < ctx->params->size; ++i){
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
                           pddl_t *pddl,
                           const pddl_params_t *params,
                           const char *err)
{
    parse_ctx_t ctx;

    ctx.types = &pddl->type;
    ctx.objs = &pddl->obj;
    ctx.preds = &pddl->pred;
    ctx.funcs = &pddl->func;
    ctx.params = params;
    ctx.err = err;

    return parse(root, &ctx, 0);
}

pddl_cond_t *pddlCondEmptyPre(void)
{
    return &condPartNew(PDDL_COND_AND)->cls;
}

pddl_cond_t *pddlCondAtomToAnd(pddl_cond_t *atom)
{
    pddl_cond_part_t *and;

    and = condPartNew(PDDL_COND_AND);
    condPartAdd(and, atom);
    return &and->cls;
}

void pddlCondPartAdd(pddl_cond_part_t *part, pddl_cond_t *c)
{
    condPartAdd(part, c);
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


static int setPredRead(pddl_cond_t *cond, void *data)
{
    pddl_cond_atom_t *atom;
    pddl_preds_t *preds = data;

    if (cond->type == PDDL_COND_ATOM){
        atom = OBJ(cond, atom);
        preds->pred[atom->pred].read = 1;
    }
    return 0;
}

void pddlCondSetPredRead(const pddl_cond_t *cond, pddl_preds_t *preds)
{
    pddlCondTraverse((pddl_cond_t *)cond, setPredRead, NULL, preds);
}


static int setPredReadWrite(pddl_cond_t *cond, void *data)
{
    pddl_cond_atom_t *atom;
    pddl_cond_when_t *when;
    pddl_preds_t *preds = data;

    if (cond->type == PDDL_COND_WHEN){
        when = OBJ(cond, when);
        pddlCondTraverse((pddl_cond_t *)when->pre, setPredRead, NULL, data);
        pddlCondTraverse((pddl_cond_t *)when->eff,
                         setPredReadWrite, NULL, data);
        return -1;

    }else if (cond->type == PDDL_COND_ATOM){
        atom = OBJ(cond, atom);
        preds->pred[atom->pred].write = 1;
    }
    return 0;
}

void pddlCondSetPredReadWriteEff(const pddl_cond_t *cond, pddl_preds_t *preds)
{
    pddlCondTraverse((pddl_cond_t *)cond, setPredReadWrite, NULL, preds);
}

/*** INSTANTIATE QUANTIFIERS ***/
struct instantiate_cond {
    int param_id;
    int obj_id;
};
typedef struct instantiate_cond instantiate_cond_t;

static int instantiateParentParam(pddl_cond_t *c, void *data)
{
    const pddl_params_t *params = data;
    pddl_cond_atom_t *a;
    int i, j;

    if (c->type == PDDL_COND_ATOM){
        a = OBJ(c, atom);
        for (i = 0; i < params->size; ++i){
            if (params->param[i].inherit < 0)
                continue;

            for (j = 0; j < a->arg_size; ++j){
                if (a->arg[j].param == i)
                    a->arg[j].param = params->param[i].inherit;
            }
        }

    }else if (c->type == PDDL_COND_ASSIGN){
        if (OBJ(c, assign)->fvalue)
            return instantiateParentParam(&OBJ(c, assign)->fvalue->cls, data);
    }

    return 0;
}

static int instantiateCond(pddl_cond_t *c, void *data)
{
    const instantiate_cond_t *d = data;
    pddl_cond_atom_t *a;
    int i;

    if (c->type == PDDL_COND_ATOM){
        a = OBJ(c, atom);
        for (i = 0; i < a->arg_size; ++i){
            if (a->arg[i].param == d->param_id){
                a->arg[i].param = -1;
                a->arg[i].obj = d->obj_id;
            }
        }

    }else if (c->type == PDDL_COND_ASSIGN){
        if (OBJ(c, assign)->fvalue)
            return instantiateCond(&OBJ(c, assign)->fvalue->cls, data);
    }

    return 0;
}

static pddl_cond_part_t *instantiatePart(pddl_cond_part_t *p,
                                         int param_id,
                                         const int *objs, int objs_size)
{
    pddl_cond_part_t *out;
    pddl_cond_t *c, *newc;
    bor_list_t *item;
    instantiate_cond_t set;
    int i;

    out = condPartNew(p->cls.type);

    for (i = 0; i < objs_size; ++i){
        BOR_LIST_FOR_EACH(&p->part, item){
            c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            newc = pddlCondClone(c);
            set.param_id = param_id;
            set.obj_id = objs[i];
            pddlCondTraverse(newc, NULL, instantiateCond, &set);
            condPartAdd(out, newc);
        }
    }

    pddlCondDel(&p->cls);
    return out;
}

static pddl_cond_t *instantiateQuant(pddl_cond_quant_t *q,
                                     const pddl_types_t *types)
{
    pddl_cond_part_t *top;
    const pddl_param_t *param;
    const int *obj;
    int i, obj_size, bval;

    // The instantiation of universal/existential quantifier is a
    // conjuction/disjunction of all instances.
    if (q->cls.type == PDDL_COND_FORALL){
        top = condPartNew(PDDL_COND_AND);
    }else{
        top = condPartNew(PDDL_COND_OR);
    }
    condPartAdd(top, q->cond);
    q->cond = NULL;

    // Apply object to each (non-inherited) parameter according to its type
    for (i = 0; i < q->param.size; ++i){
        param = q->param.param + i;
        if (param->inherit >= 0)
            continue;

        obj = pddlTypesObjsByType(types, param->type, &obj_size);
        if (obj_size == 0){
            bval = q->cls.type == PDDL_COND_FORALL;
            pddlCondDel(&top->cls);
            pddlCondDel(&q->cls);
            return &condBoolNew(bval)->cls;

        }else{
            top = instantiatePart(top, i, obj, obj_size);
        }
    }

    // Replace all parameters inherited from the parent with IDs of the
    // parent parameters.
    pddlCondTraverse(&top->cls, NULL, instantiateParentParam, &q->param);

    pddlCondDel(&q->cls);
    return &top->cls;
}

static int instantiateForall(pddl_cond_t **c, void *data)
{
    const pddl_types_t *types = data;

    if ((*c)->type != PDDL_COND_FORALL)
        return 0;

    *c = instantiateQuant(OBJ(*c, quant), types);
    return 0;
}

static int instantiateExist(pddl_cond_t **c, void *data)
{
    const pddl_types_t *types = data;

    if ((*c)->type != PDDL_COND_EXIST)
        return 0;

    *c = instantiateQuant(OBJ(*c, quant), types);
    return 0;
}

static int pddlCondInstantiateQuant(pddl_cond_t **cond,
                                    const pddl_types_t *types)
{
    int ret;

    ret = pddlCondRebuild(cond, NULL, instantiateForall, (void *)types);
    if (ret == 0)
        return pddlCondRebuild(cond, NULL, instantiateExist, (void *)types);
    return -1;
}



/*** SIMPLIFY ***/
static pddl_cond_t *removeBoolPart(pddl_cond_part_t *part)
{
    bor_list_t *item, *tmp;
    pddl_cond_t *c;
    int bval;

    BOR_LIST_FOR_EACH_SAFE(&part->part, item, tmp){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        if (c->type != PDDL_COND_BOOL)
            continue;

        bval = OBJ(c, bool)->val;
        if (part->cls.type == PDDL_COND_AND){
            if (!bval){
                pddlCondDel(&part->cls);
                return &condBoolNew(0)->cls;
            }else{
                borListDel(item);
                pddlCondDel(c);
            }

        }else{ // PDDL_COND_OR
            if (bval){
                pddlCondDel(&part->cls);
                return &condBoolNew(1)->cls;
            }else{
                borListDel(item);
                pddlCondDel(c);
            }
        }
    }

    return &part->cls;
}

static pddl_cond_t *removeBoolWhen(pddl_cond_when_t *when)
{
    pddl_cond_t *c;
    int bval;

    if (when->pre->type != PDDL_COND_BOOL)
        return &when->cls;

    bval = OBJ(when->pre, bool)->val;
    if (bval){
        c = when->eff;
        when->eff = NULL;
        pddlCondDel(&when->cls);
        return c;

    }else{ // !bval
        pddlCondDel(&when->cls);
        return &condBoolNew(1)->cls;
    }
}

static int removeBool(pddl_cond_t **c, void *data)
{

    if ((*c)->type == PDDL_COND_AND
            || (*c)->type == PDDL_COND_OR){
        *c = removeBoolPart(OBJ(*c, part));

    }else if ((*c)->type == PDDL_COND_WHEN){
        *c = removeBoolWhen(OBJ(*c, when));
    }

    return 0;
}

static pddl_cond_t *flattenPart(pddl_cond_part_t *part)
{
    bor_list_t *item, *tmp;
    pddl_cond_t *c;
    pddl_cond_part_t *p;

    if (borListEmpty(&part->part))
        return &part->cls;

    BOR_LIST_FOR_EACH_SAFE(&part->part, item, tmp){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);

        if (c->type == part->cls.type){
            // Flatten con/disjunctions
            p = OBJ(c, part);
            condPartStealPart(part, p);

            borListDel(item);
            pddlCondDel(c);

        }else if ((c->type == PDDL_COND_AND || c->type == PDDL_COND_OR)
                    && borListEmpty(&OBJ(c, part)->part)){
            borListDel(item);
            pddlCondDel(c);
        }

    }

    // If the con/disjunction contains only one atom, remove the
    // con/disjunction and return the atom directly
    if (borListPrev(&part->part) == borListNext(&part->part)){
        item = borListNext(&part->part);
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        borListDel(item);
        pddlCondDel(&part->cls);
        return c;
    }

    return &part->cls;
}

/** Splits (when ...) if its precondition is disjunction */
static pddl_cond_t *flattenWhen(pddl_cond_when_t *when)
{
    bor_list_t *item;
    pddl_cond_t *c;
    pddl_cond_part_t *pre;
    pddl_cond_part_t *and;
    pddl_cond_when_t *add;

    if (!when->pre || when->pre->type != PDDL_COND_OR)
        return &when->cls;

    and = condPartNew(PDDL_COND_AND);
    pre = OBJ(when->pre, part);
    when->pre = NULL;

    while (!borListEmpty(&pre->part)){
        item = borListNext(&pre->part);
        borListDel(item);
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        add = condWhenClone(when);
        add->pre = c;
        condPartAdd(and, &add->cls);
    }

    pddlCondDel(&pre->cls);
    pddlCondDel(&when->cls);

    return &and->cls;
}

static int flatten(pddl_cond_t **c, void *data)
{
    if ((*c)->type == PDDL_COND_AND
            || (*c)->type == PDDL_COND_OR){
        *c = flattenPart(OBJ(*c, part));

    }else if ((*c)->type == PDDL_COND_WHEN){
        *c = flattenWhen(OBJ(*c, when));
    }

    return 0;
}

static pddl_cond_part_t *moveDisjunctionsCreate1(pddl_cond_part_t *top,
                                                 pddl_cond_part_t *or)
{
    pddl_cond_part_t *ret;
    bor_list_t *item1, *item2;
    pddl_cond_t *c1, *c2;
    pddl_cond_part_t *add;

    ret = condPartNew(PDDL_COND_OR);
    BOR_LIST_FOR_EACH(&top->part, item1){
        c1 = BOR_LIST_ENTRY(item1, pddl_cond_t, conn);
        BOR_LIST_FOR_EACH(&or->part, item2){
            c2 = BOR_LIST_ENTRY(item2, pddl_cond_t, conn);
            add = OBJ(c1, part);
            add = condPartClone(add);
            condPartAdd(add, pddlCondClone(c2));
            condPartAdd(ret, &add->cls);
        }
    }

    pddlCondDel(&top->cls);
    return ret;
}

static pddl_cond_t *moveDisjunctionsCreate(pddl_cond_part_t *and,
                                           bor_list_t *or_list)
{
    bor_list_t *or_item;
    pddl_cond_part_t *or;
    pddl_cond_part_t *ret;

    ret = condPartNew(PDDL_COND_OR);
    condPartAdd(ret, &and->cls);
    while (!borListEmpty(or_list)){
        or_item = borListNext(or_list);
        borListDel(or_item);
        or = OBJ(BOR_LIST_ENTRY(or_item, pddl_cond_t, conn), part);
        ret = moveDisjunctionsCreate1(ret, or);
        pddlCondDel(&or->cls);
    }

    return &ret->cls;
}

static pddl_cond_t *moveDisjunctionsUpAnd(pddl_cond_part_t *and)
{
    bor_list_t *item, *tmp;
    bor_list_t or_list;
    pddl_cond_t *c;

    borListInit(&or_list);
    BOR_LIST_FOR_EACH_SAFE(&and->part, item, tmp){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        if (c->type != PDDL_COND_OR)
            continue;

        borListDel(item);
        borListAppend(&or_list, item);
    }

    if (borListEmpty(&or_list)){
        return &and->cls;
    }

    return moveDisjunctionsCreate(and, &or_list);
}

static int moveDisjunctionsUp(pddl_cond_t **c, void *data)
{
    if ((*c)->type == PDDL_COND_AND)
        *c = moveDisjunctionsUpAnd(OBJ(*c, part));

    if ((*c)->type == PDDL_COND_OR)
        *c = flattenPart(OBJ(*c, part));
    return 0;
}

pddl_cond_t *pddlCondNormalize(pddl_cond_t *cond, const pddl_types_t *types)
{
    pddl_cond_t *c = cond;

    // TODO: Check return values
    pddlCondInstantiateQuant(&c, types);
    pddlCondRebuild(&c, NULL, removeBool, NULL);
    pddlCondRebuild(&c, NULL, flatten, NULL);
    pddlCondRebuild(&c, NULL, moveDisjunctionsUp, NULL);
    pddlCondRebuild(&c, NULL, flatten, NULL);
    // TODO: Remove identical atoms
    return c;
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
    if (preds->pred[atom->pred].read)
        fprintf(fout, "R");
    if (preds->pred[atom->pred].write)
        fprintf(fout, "W");
    fprintf(fout, ":%s", preds->pred[atom->pred].name);

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

static void condBoolPrint(const pddl_cond_bool_t *b, FILE *fout)
{
    if (b->val){
        fprintf(fout, "TRUE");
    }else{
        fprintf(fout, "FALSE");
    }
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

    }else if (cond->type == PDDL_COND_BOOL){
        condBoolPrint(OBJ(cond, bool), fout);

    }else{
        fprintf(stderr, "Fatal Error: Unknown type!\n");
        exit(-1);
    }
}
