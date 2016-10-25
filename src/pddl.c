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
#include <pddl/pddl.h>
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

pddl_t *pddlNew(const char *domain_fn, const char *problem_fn)
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
    pddl->domain_lisp = domain_lisp;
    pddl->problem_lisp = problem_lisp;
    pddl->domain_name = parseDomainName(&domain_lisp->root);
    if (pddl->domain_name == NULL)
        goto pddl_fail;

    pddl->problem_name = parseProblemName(&problem_lisp->root);
    if (pddl->domain_name == NULL)
        goto pddl_fail;


    if (checkDomainName(pddl) != 0
            || pddlRequireParse(domain_lisp, &pddl->require) != 0
            || pddlTypesParse(domain_lisp, &pddl->type) != 0
            || pddlObjsParse(domain_lisp, problem_lisp,
                                 &pddl->type, pddl->require, &pddl->obj) != 0
            || pddlTypeObjInit(&pddl->type_obj, &pddl->type, &pddl->obj) != 0
            || pddlPredsParse(domain_lisp, pddl->require,
                                       &pddl->type, &pddl->predicate) != 0
            || pddlFunctionsParse(domain_lisp, &pddl->type,
                                      &pddl->function) != 0
            || pddlFactsParseInit(problem_lisp, &pddl->predicate,
                                      &pddl->function, &pddl->obj,
                                      &pddl->init_fact, &pddl->init_func) != 0
            || pddlFactsParseGoal(problem_lisp, &pddl->predicate,
                                      &pddl->obj, &pddl->goal) != 0
            || pddlActionsParse(domain_lisp, &pddl->type, &pddl->obj,
                                    &pddl->type_obj, &pddl->predicate,
                                    &pddl->function, pddl->require,
                                    &pddl->action) != 0
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
    pddlTypeObjFree(&pddl->type_obj);
    pddlPredsFree(&pddl->predicate);
    pddlPredsFree(&pddl->function);
    pddlFactsFree(&pddl->init_fact);
    pddlFactsFree(&pddl->init_func);
    pddlFactsFree(&pddl->goal);
    pddlActionsFree(&pddl->action);

    BOR_FREE(pddl);
}


void pddlDump(const pddl_t *pddl, FILE *fout)
{
    fprintf(fout, "Domain: %s\n", pddl->domain_name);
    fprintf(fout, "Problem: %s\n", pddl->problem_name);
    fprintf(fout, "Require: %x\n", pddl->require);
    pddlTypesPrint(&pddl->type, fout);
    pddlObjsPrint(&pddl->obj, fout);
    pddlTypeObjPrint(&pddl->type_obj, fout);
    pddlPredsPrint(&pddl->predicate, "Predicate", fout);
    pddlPredsPrint(&pddl->function, "Function", fout);
    pddlActionsPrint(&pddl->action, &pddl->obj, &pddl->predicate,
                         &pddl->function, fout);

    pddlFactsPrintGoal(&pddl->predicate, &pddl->obj, &pddl->goal, fout);
    pddlFactsPrintInit(&pddl->predicate, &pddl->obj, &pddl->init_fact, fout);
    pddlFactsPrintInitFunc(&pddl->function, &pddl->obj, &pddl->init_func, fout);
    fprintf(fout, "Metric: %d\n", pddl->metric);
}
