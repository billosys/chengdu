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

struct _set_t {
    pddl_pred_t *pred;
    const pddl_types_t *types;
    const char *owner_var;
};
typedef struct _set_t set_t;

static const char *eq_name = "=";

static int setCB(const pddl_lisp_node_t *root,
                 int child_from, int child_to, int child_type, void *ud)
{
    pddl_pred_t *pred = ((set_t *)ud)->pred;
    const pddl_types_t *types = ((set_t *)ud)->types;
    const char *owner_var = ((set_t *)ud)->owner_var;
    int i, j, tid;

    tid = 0;
    if (child_type >= 0){
        tid = pddlTypesGet(types, root->child[child_type].value);
        if (tid < 0){
            ERRN(root->child + child_type, "Invalid type `%s'",
                 root->child[child_type].value);
            return -1;
        }
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

static int parsePred(const pddl_t *pddl,
                     const pddl_lisp_node_t *n,
                     const char *owner_var,
                     const char *errname,
                     pddl_preds_t *ps)
{
    pddl_pred_t *p;
    set_t set;

    if (n->child_size < 1 || n->child[0].value == NULL){
        ERRN(n, "Invalid definition of %s.", errname);
        return -1;
    }

    if (checkDuplicate(ps, n->child[0].value)){
        ERRN(n, "Duplicate %s `%s'", errname, n->child[0].value);
        return -1;
    }

    p = pddlPredsAdd(ps);
    set.pred = p;
    set.types = &pddl->type;
    set.owner_var = owner_var;
    if (pddlLispParseTypedList(n, 1, n->child_size, setCB, &set) != 0){
        pddlPredsRemoveLast(ps);
        return -1;
    }

    p->name = n->child[0].value;
    return 0;
}

static int parsePrivatePreds(const pddl_t *pddl,
                             const pddl_lisp_node_t *n,
                             pddl_preds_t *ps)
{
    const char *owner_var;
    int factor, i, from;

    factor = (pddl->require & PDDL_REQUIRE_FACTORED_PRIVACY);

    if (factor){
        if (n->child_size < 2
                || n->child[0].kw != PDDL_KW_PRIVATE){
            ERRN2(n, "Invalid definition of :private predicate.");
            return -1;
        }

        owner_var = NULL;
        from = 1;

    }else{
        if (n->child_size < 3
                || n->child[0].kw != PDDL_KW_PRIVATE
                || n->child[1].value == NULL
                || n->child[1].value[0] != '?'
                || (n->child[2].value != NULL && n->child_size < 5)){
            ERRN2(n, "Invalid definition of :private predicate.");
            return -1;
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
            return -1;
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
    if (pddl->require & PDDL_REQUIRE_EQUALITY)
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
            return -1;
    }

    if (private){
        // Parse :private predicates
        for (i = to; i < n->child_size; ++i){
            if (parsePrivatePreds(pddl, n->child + i, &pddl->pred) != 0)
                return -1;
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
            return -1;

        if (i + 2 < n->child_size
                && n->child[i + 1].value != NULL
                && strcmp(n->child[i + 1].value, "-") == 0){
            if (n->child[i + 2].value == NULL
                    || strcmp(n->child[i + 2].value, "number") != 0){
                ERRN2(n->child + i + 2, "Only number functions are supported.");
                return -1;
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
        fprintf(fout, "\n");
    }
}
