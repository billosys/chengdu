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
#include "err.h"

static const char *parseName(pddl_lisp_node_t *root, int kw,
                             const char *err_name)
{
    const pddl_lisp_node_t *n;

    n = pddlLispFindNode(root, kw);
    if (n == NULL){
        ERR("Could not find %s name definition.", err_name);
        return NULL;
    }

    if (n->child_size != 2 || n->child[1].value == NULL){
        ERRN(n, "Invalid %s name definition", err_name);
        return NULL;
    }

    return n->child[1].value;
}

static const char *parseDomainName(pddl_lisp_node_t *root)
{
    return parseName(root, PDDL_KW_DOMAIN, "domain");
}

static const char *parseProblemName(pddl_lisp_node_t *root)
{
    return parseName(root, PDDL_KW_PROBLEM, "problem");
}

static int checkDomainName(pddl_t *pddl)
{
    const char *problem_domain_name;

    problem_domain_name = parseName(&pddl->problem_lisp->root,
                                    PDDL_KW_DOMAIN, ":domain");
    if (problem_domain_name == NULL
            || strcmp(problem_domain_name, pddl->domain_name) != 0){
        WARN("Domain names does not match: `%s' x `%s'",
             pddl->domain_name, problem_domain_name);
        return 0;
    }
    return 0;
}

static int parseMetric(pddl_t *pddl, const pddl_lisp_node_t *root)
{
    const pddl_lisp_node_t *n;

    n = pddlLispFindNode(root, PDDL_KW_METRIC);
    if (n == NULL)
        return 0;

    if (n->child_size != 3
            || n->child[1].value == NULL
            || n->child[1].kw != PDDL_KW_MINIMIZE
            || n->child[2].value != NULL
            || n->child[2].child_size != 1
            || strcmp(n->child[2].child[0].value, "total-cost") != 0){
        ERRN2(n, "Only (:metric minimize (total-cost)) is supported.");
        return -1;
    }

    pddl->metric = 1;
    return 0;
}

static int parseGoal(pddl_t *pddl)
{
    const pddl_lisp_node_t *ngoal;

    ngoal = pddlLispFindNode(&pddl->problem_lisp->root, PDDL_KW_GOAL);
    if (ngoal == NULL){
        ERR2("Missing :goal.");
        return -1;
    }

    if (ngoal->child_size != 2
            || ngoal->child[1].value != NULL){
        ERRN2(ngoal, "Invalid definition of :goal.");
        return -1;
    }

    pddl->goal = pddlCondParse(ngoal->child + 1, pddl, NULL, ":goal");
    if (pddl->goal == NULL)
        return -1;
    return 0;
}

pddl_t *pddlNew(const char *domain_fn, const char *problem_fn,
                unsigned flags)
{
    pddl_t *pddl;
    pddl_lisp_t *domain_lisp, *problem_lisp;

    domain_lisp = pddlLispParse(domain_fn);
    problem_lisp = pddlLispParse(problem_fn);
    if (domain_lisp == NULL || problem_lisp == NULL){
        if (domain_lisp)
            pddlLispDel(domain_lisp);
        if (problem_lisp)
            pddlLispDel(problem_lisp);
        return NULL;
    }

    pddl = BOR_ALLOC(pddl_t);
    bzero(pddl, sizeof(*pddl));
    pddlFactsInit(&pddl->init_fact);
    pddlFactsInit(&pddl->init_func);
    pddl->flags = flags;
    pddl->domain_lisp = domain_lisp;
    pddl->problem_lisp = problem_lisp;
    pddl->domain_name = parseDomainName(&domain_lisp->root);
    if (pddl->domain_name == NULL)
        goto pddl_fail;

    pddl->problem_name = parseProblemName(&problem_lisp->root);
    if (pddl->domain_name == NULL)
        goto pddl_fail;


    if (checkDomainName(pddl) != 0
            || pddlRequireParse(pddl) != 0
            || pddlTypesParse(pddl) != 0
            || pddlObjsParse(pddl) != 0
            || pddlPredsParse(pddl) != 0
            || pddlFuncsParse(pddl) != 0
            || pddlFactsParseInit(pddl) != 0
            || parseGoal(pddl) != 0
            || pddlActionsParse(pddl) != 0
            || parseMetric(pddl, &problem_lisp->root) != 0){
        goto pddl_fail;
    }

    return pddl;

pddl_fail:
    if (pddl != NULL)
        pddlDel(pddl);
    return NULL;
}

void pddlDel(pddl_t *pddl)
{
    if (pddl->domain_lisp)
        pddlLispDel(pddl->domain_lisp);
    if (pddl->problem_lisp)
        pddlLispDel(pddl->problem_lisp);
    pddlTypesFree(&pddl->type);
    pddlObjsFree(&pddl->obj);
    pddlPredsFree(&pddl->pred);
    pddlPredsFree(&pddl->func);
    pddlFactsFree(&pddl->init_fact);
    pddlFactsFree(&pddl->init_func);
    if (pddl->goal)
        pddlCondDel(pddl->goal);
    pddlActionsFree(&pddl->action);

    BOR_FREE(pddl);
}

static int markNegPre(pddl_cond_t *c, void *_m)
{
    pddl_cond_atom_t *atom;
    int *m = _m;

    if (c->type == PDDL_COND_ATOM){
        atom = PDDL_COND_CAST(c, atom);
        if (atom->neg)
            m[atom->pred] = 1;
    }

    return 0;
}

static int markNegPreWhen(pddl_cond_t *c, void *_m)
{
    pddl_cond_when_t *when;

    if (c->type == PDDL_COND_WHEN){
        when = PDDL_COND_CAST(c, when);
        pddlCondTraverse(when->pre, markNegPre, NULL, _m);
    }

    return 0;
}

/** Sets to 1 indexes in {np} of those predicates that are not static and
 *  appear as negative preconditions */
static void findNonStaticPredInNegPre(pddl_t *pddl, int *np)
{
    int i;

    bzero(np, sizeof(int) * pddl->pred.size);
    for (i = 0; i < pddl->action.size; ++i){
        pddlCondTraverse(pddl->action.action[i].pre, markNegPre, NULL, np);
        pddlCondTraverse(pddl->action.action[i].eff, markNegPreWhen, NULL, np);
    }
    for (i = 0; i < pddl->pred.size; ++i){
        if (pddlPredIsStatic(pddl->pred.pred + i))
            np[i] = 0;
    }
}

/** Create a new NOT-... predicate and returns its ID */
static int createNewNotPred(pddl_t *pddl, int pred_id)
{
    const pddl_pred_t *pos = pddl->pred.pred + pred_id;
    pddl_pred_t *neg;
    int name_size;
    char *name;

    name_size = strlen(pos->name) + 4;
    name = BOR_ALLOC_ARR(char, name_size + 1);
    strcpy(name, "NOT-");
    strcpy(name + 4, pos->name);

    neg = pddlPredsAdd(&pddl->pred);
    // pddlPredsAdd() can reallocate, so we need to probe positive
    // predicate again
    pos = pddl->pred.pred + pred_id;
    pddlPredCopy(neg, pos);
    if (neg->free_name)
        BOR_FREE((char *)neg->name);
    neg->name = name;
    neg->free_name = 1;

    return pddl->pred.size - 1;
}

static int replaceNegPre(pddl_cond_t **c, void *_ids)
{
    int *ids = _ids;
    int pos = ids[0];
    int neg = ids[1];
    pddl_cond_atom_t *atom;

    if ((*c)->type == PDDL_COND_ATOM){
        atom = PDDL_COND_CAST(*c, atom);
        if (atom->pred == pos && atom->neg){
            atom->pred = neg;
            atom->neg = 0;
        }
    }

    return 0;
}

static int replaceNegEff(pddl_cond_t **c, void *_ids)
{
    int *ids = _ids;
    int pos = ids[0];
    int neg = ids[1];
    pddl_cond_t *c2;
    pddl_cond_atom_t *atom, *not_atom;
    pddl_cond_when_t *when;
    pddl_cond_part_t *and;

    if ((*c)->type == PDDL_COND_WHEN){
        when = PDDL_COND_CAST(*c, when);
        pddlCondRebuild(&when->pre, NULL, replaceNegPre, _ids);
        pddlCondRebuild(&when->eff, replaceNegEff, NULL, _ids);
        return -1;

    }else if ((*c)->type == PDDL_COND_ATOM){
        atom = PDDL_COND_CAST(*c, atom);
        if (atom->pred == pos){
            // Create new NOT atom and flip negation
            c2 = pddlCondClone(*c);
            not_atom = PDDL_COND_CAST(c2, atom);
            not_atom->pred = neg;
            not_atom->neg = !atom->neg;

            // Transorm atom to (and atom)
            *c = pddlCondAtomToAnd(*c);
            and = PDDL_COND_CAST(*c, part);
            pddlCondPartAdd(and, c2);

            // Prevent recursion
            return -1;
        }
    }

    return 0;
}

static void compileOutNegPreInAction(pddl_t *pddl, int pos, int neg,
                                     pddl_action_t *a)
{
    int ids[2] = { pos, neg };
    pddlCondRebuild(&a->pre, NULL, replaceNegPre, ids);
    pddlCondRebuild(&a->eff, replaceNegEff, NULL, ids);
    pddlActionNormalize(a, &pddl->type);
}

static void compileOutNegPre(pddl_t *pddl, int pos, int neg)
{
    int i;

    for (i = 0; i < pddl->action.size; ++i)
        compileOutNegPreInAction(pddl, pos, neg, pddl->action.action + i);
}

static void addNotPredsToInitRec(pddl_t *pddl, int pos, int neg,
                                 pddl_fact_t *fact,
                                 const pddl_pred_t *pred, int argi)
{
    const int *obj;
    int i, obj_size;

    if (argi == fact->arg_size){
        if (pddlFactSetPrivate(pddl, fact) != 0){
            fprintf(stderr, "Error PDDL: Could not determine privateness of ");
            pddlFactPrint(pddl, fact, stderr);
            fprintf(stderr, ".\n");
            ERR2("The fact is defined so it should be private for two"
                 " different agents.");
        }

        fact->pred = pos;
        if (pddlFactsFind(&pddl->init_fact, fact) < 0){
            fact->pred = neg;
            pddlFactsAdd(&pddl->init_fact, fact);
        }

        return;
    }

    obj = pddlTypesObjsByType(&pddl->type, pred->param[argi], &obj_size);
    for (i = 0; i < obj_size; ++i){
        fact->arg[argi] = obj[i];
        addNotPredsToInitRec(pddl, pos, neg, fact, pred, argi + 1);
    }
}

static void addNotPredsToInit(pddl_t *pddl, int pos, int neg)
{
    const pddl_pred_t *pos_pred = pddl->pred.pred + pos;
    pddl_fact_t fact;
    int arg[pos_pred->param_size];

    // Prepare fact object
    pddlFactInit(&fact);
    fact.arg = arg;
    fact.arg_size = pos_pred->param_size;

    // Recursivelly try all possible objects for each argument
    addNotPredsToInitRec(pddl, pos, neg, &fact, pos_pred, 0);
}

/** Compile out negative preconditions if they are not static */
static void compileOutNonStaticNegPre(pddl_t *pddl)
{
    int i, size, *negpred;
    int not;

    size = pddl->pred.size;
    negpred = BOR_ALLOC_ARR(int, size);
    findNonStaticPredInNegPre(pddl, negpred);

    for (i = 0; i < size; ++i){
        if (negpred[i]){
            not = createNewNotPred(pddl, i);
            compileOutNegPre(pddl, i, not);
            addNotPredsToInit(pddl, i, not);
        }
    }
    BOR_FREE(negpred);
}


void pddlNormalize(pddl_t *pddl)
{
    int i;

    for (i = 0; i < pddl->action.size; ++i)
        pddlActionNormalize(pddl->action.action + i, &pddl->type);

    for (i = 0; i < pddl->action.size; ++i)
        pddlActionSplit(pddl->action.action + i, &pddl->action);

#ifdef PDDL_DEBUG
    for (i = 0; i < pddl->action.size; ++i)
        pddlActionAssertPreConjuction(pddl->action.action + i);
#endif

    if (pddl->goal)
        pddl->goal = pddlCondNormalize(pddl->goal, &pddl->type);

    compileOutNonStaticNegPre(pddl);
}

void pddlDump(const pddl_t *pddl, FILE *fout)
{
    fprintf(fout, "Domain: %s\n", pddl->domain_name);
    fprintf(fout, "Problem: %s\n", pddl->problem_name);
    fprintf(fout, "Require: %x\n", pddl->require);
    pddlTypesPrint(&pddl->type, fout);
    pddlObjsPrint(&pddl->obj, fout);
    pddlPredsPrint(&pddl->pred, "Predicate", fout);
    pddlPredsPrint(&pddl->func, "Function", fout);
    pddlActionsPrint(&pddl->action, &pddl->obj, &pddl->pred,
                         &pddl->func, fout);

    pddlFactsPrintInit(pddl, &pddl->init_fact, fout);
    pddlFactsPrintInitFunc(pddl, &pddl->init_func, fout);

    fprintf(fout, "Goal: ");
    pddlCondPrint(pddl->goal, &pddl->obj, &pddl->pred, &pddl->func, NULL, fout);
    fprintf(fout, "\n");

    fprintf(fout, "Metric: %d\n", pddl->metric);
}
