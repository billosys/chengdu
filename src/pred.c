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
#include "pddl/pred.h"
#include "err.h"

void pddlPredCopy(pddl_pred_t *dst, const pddl_pred_t *src)
{
    memcpy(dst, src, sizeof(*src));
    if (src->param != NULL){
        dst->param = BOR_ALLOC_ARR(int, src->param_size);
        memcpy(dst->param, src->param, sizeof(int) * src->param_size);
    }

    if (src->free_name){
        dst->name = BOR_STRDUP(src->name);
    }
}

struct _set_t {
    pddl_pred_t *pred;
    pddl_types_t *types;
    const char *owner_var;
};
typedef struct _set_t set_t;

static const char *eq_name = "=";

static int setCB(const pddl_lisp_node_t *root,
                 int child_from, int child_to, int child_type, void *ud)
{
    pddl_pred_t *pred = ((set_t *)ud)->pred;
    pddl_types_t *types = ((set_t *)ud)->types;
    const char *owner_var = ((set_t *)ud)->owner_var;
    int i, j, tid;

    tid = 0;
    if (child_type >= 0){
        if ((tid = pddlTypeFromLispNode(types, root->child + child_type)) < 0)
            return -1;
    }

    j = pred->param_size;
    pred->param_size += child_to - child_from;
    pred->param = BOR_REALLOC_ARR(pred->param, int, pred->param_size);
    for (i = child_from; i < child_to; ++i, ++j){
        pred->param[j] = tid;
        if (owner_var != NULL && strcmp(owner_var, root->child[i].value) == 0){
            pred->owner_param = j;
        }
    }
    return 0;
}

static int checkDuplicate(const pddl_preds_t *ps, const char *name)
{
    int i;

    for (i = 0; i < ps->size; ++i){
        if (strcmp(ps->pred[i].name, name) == 0)
            return 1;
    }
    return 0;
}

static int parsePred(pddl_t *pddl,
                     const pddl_lisp_node_t *n,
                     const char *owner_var,
                     const char *errname,
                     pddl_preds_t *ps)
{
    pddl_pred_t *p;
    set_t set;

    if (n->child_size < 1 || n->child[0].value == NULL)
        ERR_LISP_RET(-1, n, "Invalid %s", errname);

    if (checkDuplicate(ps, n->child[0].value)){
        // TODO: err/warn
        ERR_LISP_RET(-1, n, "Duplicate %s `%s'", errname, n->child[0].value);
    }

    p = pddlPredsAdd(ps);
    set.pred = p;
    set.types = &pddl->type;
    set.owner_var = owner_var;
    if (pddlLispParseTypedList(n, 1, n->child_size, setCB, &set) != 0){
        pddlPredsRemoveLast(ps);
        TRACE_UPDATE_RET(-1, "%s `%s': ", errname, n->child[0].value);
    }

    p->name = n->child[0].value;
    return 0;
}

static int parsePrivatePreds(pddl_t *pddl,
                             const pddl_lisp_node_t *n,
                             pddl_preds_t *ps)
{
    const char *owner_var;
    int factor, i, from;

    factor = (pddl->require & PDDL_REQUIRE_FACTORED_PRIVACY);

    if (factor){
        if (n->child_size < 2 || n->child[0].kw != PDDL_KW_PRIVATE)
            ERR_LISP_RET2(-1, n, "Invalid definition of :private predicate");

        owner_var = NULL;
        from = 1;

    }else{
        if (n->child_size < 3
                || n->child[0].kw != PDDL_KW_PRIVATE
                || n->child[1].value == NULL
                || n->child[1].value[0] != '?'
                || (n->child[2].value != NULL && n->child_size < 5)){
            ERR_LISP_RET2(-1, n, "Invalid definition of :private predicate");
        }

        owner_var = n->child[1].value;

        if (n->child[2].value == NULL){
            from = 2;
        }else{
            from = 4;
        }
    }

    for (i = from; i < n->child_size; ++i){
        if (parsePred(pddl, n->child + i, owner_var,
                      "private predicate", ps) != 0){
            TRACE_RET(-1);
        }

        ps->pred[ps->size - 1].is_private = 1;
    }

    return 0;
}

static void addEqPredicate(pddl_preds_t *ps)
{
    pddl_pred_t *p;

    p = pddlPredsAdd(ps);
    p->name = eq_name;
    p->param_size = 2;
    p->param = BOR_CALLOC_ARR(int, 2);
    ps->eq_pred = ps->size - 1;
}

int pddlPredsParse(pddl_t *pddl)
{
    const pddl_lisp_node_t *n;
    int i, to, private;

    n = pddlLispFindNode(&pddl->domain_lisp->root, PDDL_KW_PREDICATES);
    if (n == NULL)
        return 0;

    pddl->pred.eq_pred = -1;
    addEqPredicate(&pddl->pred);

    // Determine if we can expect :private definitions
    private = (pddl->require & PDDL_REQUIRE_UNFACTORED_PRIVACY)
                || (pddl->require & PDDL_REQUIRE_FACTORED_PRIVACY);

    if (private){
        // Find out first :private definition
        for (to = 1; to < n->child_size; ++to){
            if (n->child[to].child_size > 0
                    && n->child[to].child[0].kw == PDDL_KW_PRIVATE)
                break;
        }
    }else{
        to = n->child_size;
    }

    // Parse non :private predicates
    for (i = 1; i < to; ++i){
        if (parsePred(pddl, n->child + i, NULL, "predicate", &pddl->pred) != 0)
            TRACE_UPDATE_RET(-1, "While parsing :predicates in %s: ",
                             pddl->domain_lisp->filename);
    }

    if (private){
        // Parse :private predicates
        for (i = to; i < n->child_size; ++i){
            if (parsePrivatePreds(pddl, n->child + i, &pddl->pred) != 0)
                TRACE_UPDATE_RET(-1, "While parsing private :predicates in"
                                 " %s: ", pddl->domain_lisp->filename);
        }
    }

    return 0;
}

int pddlFuncsParse(pddl_t *pddl)
{
    const pddl_lisp_node_t *n;
    int i;

    n = pddlLispFindNode(&pddl->domain_lisp->root, PDDL_KW_FUNCTIONS);
    if (n == NULL)
        return 0;

    for (i = 1; i < n->child_size; ++i){
        if (parsePred(pddl, n->child + i, NULL, "function", &pddl->func) != 0)
            TRACE_UPDATE_RET(-1, "While parsing :functions in %s: ",
                             pddl->domain_lisp->filename);

        if (i + 2 < n->child_size
                && n->child[i + 1].value != NULL
                && strcmp(n->child[i + 1].value, "-") == 0){
            if (n->child[i + 2].value == NULL
                    || strcmp(n->child[i + 2].value, "number") != 0){
                ERR_RET(-1, "While parsing :functions in %s: Only number"
                            " functions are supported (line %d).",
                        pddl->domain_lisp->filename, n->child[i + 2].lineno);
            }
            i += 2;
        }
    }

    return 0;
}

void pddlPredsFree(pddl_preds_t *ps)
{
    int i;

    for (i = 0; i < ps->size; ++i){
        if (ps->pred[i].param != NULL)
            BOR_FREE(ps->pred[i].param);
        if (ps->pred[i].free_name)
            BOR_FREE((char *)ps->pred[i].name);
    }
    if (ps->pred != NULL)
        BOR_FREE(ps->pred);
}

int pddlPredsGet(const pddl_preds_t *ps, const char *name)
{
    int i;

    for (i = 0; i < ps->size; ++i){
        if (strcmp(ps->pred[i].name, name) == 0)
            return i;
    }
    return -1;
}

pddl_pred_t *pddlPredsAdd(pddl_preds_t *ps)
{
    pddl_pred_t *p;

    if (ps->size >= ps->alloc){
        if (ps->alloc == 0){
            ps->alloc = 2;
        }else{
            ps->alloc *= 2;
        }
        ps->pred = BOR_REALLOC_ARR(ps->pred, pddl_pred_t,
                                   ps->alloc);
    }

    p = ps->pred + ps->size++;
    bzero(p, sizeof(*p));
    p->owner_param = -1;
    p->neg_of = -1;
    return p;
}

void pddlPredsRemoveLast(pddl_preds_t *ps)
{
    pddl_pred_t *p;

    p = ps->pred + --ps->size;
    if (p->param != NULL)
        BOR_FREE(p->param);
}

void pddlPredsPrint(const pddl_preds_t *ps,
                    const char *title, FILE *fout)
{
    int i, j;

    fprintf(fout, "%s[%d]:\n", title, ps->size);
    for (i = 0; i < ps->size; ++i){
        fprintf(fout, "    %s:", ps->pred[i].name);
        for (j = 0; j < ps->pred[i].param_size; ++j){
            fprintf(fout, " %d", ps->pred[i].param[j]);
        }
        fprintf(fout, " :: is-private: %d, owner-param: %d",
                ps->pred[i].is_private, ps->pred[i].owner_param);
        fprintf(fout, ", read: %d, write: %d",
                ps->pred[i].read, ps->pred[i].write);
        if (ps->pred[i].neg_of >= 0)
            fprintf(fout, ", neg-of: %d", ps->pred[i].neg_of);
        fprintf(fout, "\n");
    }
}

static void printPDDL(const char *t,
                      const pddl_preds_t *ps,
                      const pddl_types_t *ts,
                      FILE *fout)
{
    if (ps->size == 0)
        return;
    fprintf(fout, "(:%s\n", t);
    for (int i = 0; i < ps->size; ++i){
        const pddl_pred_t *p = ps->pred + i;
        fprintf(fout, "    (%s", p->name);
        for (int j = 0; j < p->param_size; ++j){
            fprintf(fout, " ?x%d - %s", j, ts->type[p->param[j]].name);
        }
        fprintf(fout, ")\n");
    }
    fprintf(fout, ")\n");
}

void pddlPredsPrintPDDL(const pddl_preds_t *ps,
                        const pddl_types_t *ts,
                        FILE *fout)
{
    printPDDL("predicates", ps, ts, fout);
}

void pddlFuncsPrintPDDL(const pddl_preds_t *ps,
                        const pddl_types_t *ts,
                        FILE *fout)
{
    printPDDL("functions", ps, ts, fout);
}
