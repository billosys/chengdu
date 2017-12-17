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
#include "pddl/err.h"
#include "err.h"

static int checkConfig(const pddl_config_t *cfg)
{
    if (cfg->compile_away_cond_eff && !cfg->normalize){
        ERR_RET2(0, "Config: compile_away_conf_eff requires setting"
                    " normalize to true.");
    }

    if (cfg->strips && !cfg->normalize){
        ERR_RET2(0, "Config: cannot translate PDDL to STRIPS without"
                    " normalization.");
    }

    if (cfg->fdr && !cfg->strips){
        ERR_RET2(0, "Config: cannot translate PDDL to FDR without first"
                    " translating it into STRIPS.");
    }

    return 1;
}

static const char *parseName(pddl_lisp_t *lisp, int kw,
                             const char *err_name)
{
    const pddl_lisp_node_t *n;

    n = pddlLispFindNode(&lisp->root, kw);
    if (n == NULL){
        // TODO: Configure warn/err
        ERR_RET(NULL, "Could not find %s name definition in %s.",
                err_name, lisp->filename);
    }

    if (n->child_size != 2 || n->child[1].value == NULL){
        ERR_RET(NULL, "Invalid %s name definition in %s.",
                err_name, lisp->filename);
    }

    return n->child[1].value;
}

static const char *parseDomainName(pddl_lisp_t *lisp)
{
    return parseName(lisp, PDDL_KW_DOMAIN, "domain");
}

static const char *parseProblemName(pddl_lisp_t *lisp)
{
    return parseName(lisp, PDDL_KW_PROBLEM, "problem");
}

static int checkDomainName(pddl_t *pddl)
{
    const char *problem_domain_name;

    // TODO: Configure err/warn/nothing
    problem_domain_name = parseName(pddl->problem_lisp,
                                    PDDL_KW_DOMAIN2, ":domain");
    if (problem_domain_name == NULL)
        TRACE_RET(0);

    if (strcmp(problem_domain_name, pddl->domain_name) != 0){
        WARN("Domain names does not match: `%s' x `%s'",
             pddl->domain_name, problem_domain_name);
        return 0;
    }
    return 0;
}

static int parseMetric(pddl_t *pddl, const pddl_lisp_t *lisp)
{
    const pddl_lisp_node_t *n;

    n = pddlLispFindNode(&lisp->root, PDDL_KW_METRIC);
    if (n == NULL)
        return 0;

    if (n->child_size != 3
            || n->child[1].value == NULL
            || n->child[1].kw != PDDL_KW_MINIMIZE
            || n->child[2].value != NULL
            || n->child[2].child_size != 1
            || strcmp(n->child[2].child[0].value, "total-cost") != 0){
        ERR_RET(-1, "Only (:metric minimize (total-cost)) is supported"
                    " (line %d in %s).", n->lineno, lisp->filename);
    }

    pddl->metric = 1;
    return 0;
}

static int parseInit(pddl_t *pddl)
{
    const pddl_lisp_node_t *ninit;

    ninit = pddlLispFindNode(&pddl->problem_lisp->root, PDDL_KW_INIT);
    if (ninit == NULL)
        ERR_RET(-1, "Missing :init in %s.", pddl->problem_lisp->filename);

    pddl->init = pddlCondParseInit(ninit, pddl);
    if (pddl->init == NULL){
        TRACE_UPDATE_RET(-1, "While parsing :init specification in %s: ",
                         pddl->problem_lisp->filename);
    }
    return 0;
}

static int parseGoal(pddl_t *pddl)
{
    const pddl_lisp_node_t *ngoal;

    ngoal = pddlLispFindNode(&pddl->problem_lisp->root, PDDL_KW_GOAL);
    if (ngoal == NULL)
        ERR_RET(-1, "Missing :goal in %s.", pddl->problem_lisp->filename);

    if (ngoal->child_size != 2 || ngoal->child[1].value != NULL){
        ERR_RET(-1, "Invalid definition of :goal in %s (line %d).",
                pddl->problem_lisp->filename, ngoal->lineno);
    }

    pddl->goal = pddlCondParse(ngoal->child + 1, pddl, NULL, "");
    if (pddl->goal == NULL){
        TRACE_UPDATE_RET(-1, "While parsing :goal specification in %s: ",
                         pddl->problem_lisp->filename);
    }
    return 0;
}

pddl_t *pddlNew(const char *domain_fn, const char *problem_fn,
                const pddl_config_t *cfg)
{
    pddl_t *pddl;
    pddl_lisp_t *domain_lisp, *problem_lisp;

    INFO("Translation of %s and %s.", domain_fn, problem_fn);

    if (!checkConfig(cfg))
        TRACE_RET(NULL);

    INFO2("Parsing domain lisp file...");
    domain_lisp = pddlLispParse(domain_fn);
    if (domain_lisp == NULL)
        TRACE_RET(NULL);

    INFO2("Parsing problem lisp file...");
    problem_lisp = pddlLispParse(problem_fn);
    if (problem_lisp == NULL){
        if (domain_lisp)
            pddlLispDel(domain_lisp);
        TRACE_RET(NULL);
    }

    pddl = BOR_ALLOC(pddl_t);
    bzero(pddl, sizeof(*pddl));

    pddl->cfg = *cfg;
    // FDR requires mutex groups so enable it if not already enabled
    if (pddl->cfg.fdr)
        pddl->cfg.strips_cfg.fa_mgroup = 1;

    INFO2("Parsing entire contents of domain/problem PDDL...");
    pddl->domain_lisp = domain_lisp;
    pddl->problem_lisp = problem_lisp;
    pddl->domain_name = parseDomainName(domain_lisp);
    if (pddl->domain_name == NULL)
        goto pddl_fail;

    pddl->problem_name = parseProblemName(problem_lisp);
    if (pddl->domain_name == NULL)
        goto pddl_fail;


    if (checkDomainName(pddl) != 0
            || pddlRequireParse(pddl) != 0
            || pddlTypesParse(pddl) != 0
            || pddlObjsParse(pddl) != 0
            || pddlPredsParse(pddl) != 0
            || pddlFuncsParse(pddl) != 0
            || parseInit(pddl) != 0
            || parseGoal(pddl) != 0
            || pddlActionsParse(pddl) != 0
            || parseMetric(pddl, problem_lisp) != 0){
        goto pddl_fail;
    }
    INFO2("PDDL content parsed.");

    if (pddl->cfg.compile_away_cond_eff){
        // It does normalization so we can skip pddlNormalize().
        pddlCompileAwayCondEff(pddl);
        INFO2("Conditional effects compiled away.");

    }else if (pddl->cfg.normalize){
        pddlNormalize(pddl);
        INFO2("PDDL problem is normalized.");
    }

    if (pddl->cfg.strips){
        pddl->strips = pddlStripsNew(pddl, &pddl->cfg.strips_cfg);
        if (pddl->strips == NULL)
            goto pddl_fail;
    }

    if (pddl->cfg.fdr){
        pddl->fdr = pddlFDRFromStrips(pddl->strips, &pddl->strips->mgroup,
                                      pddl->cfg.fdr_vars_flags);
        if (pddl->fdr == NULL)
            goto pddl_fail;
    }

    return pddl;

pddl_fail:
    if (pddl != NULL)
        pddlDel(pddl);
    TRACE_RET(NULL);
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
    if (pddl->init)
        pddlCondDel(&pddl->init->cls);
    if (pddl->goal)
        pddlCondDel(pddl->goal);
    pddlActionsFree(&pddl->action);

    if (pddl->strips != NULL)
        pddlStripsDel(pddl->strips);
    if (pddl->fdr != NULL)
        pddlFDRDel(pddl->fdr);

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
    pddl_pred_t *pos = pddl->pred.pred + pred_id;
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
    neg->neg_of = pred_id;
    pos->neg_of = pddl->pred.size - 1;

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
    pddlActionNormalize(a, pddl);
}

static void compileOutNegPre(pddl_t *pddl, int pos, int neg)
{
    int i;

    for (i = 0; i < pddl->action.size; ++i)
        compileOutNegPreInAction(pddl, pos, neg, pddl->action.action + i);
}

static int initHasFact(const pddl_t *pddl, int pred,
                       int arg_size, const int *arg)
{
    bor_list_t *item;
    const pddl_cond_t *c;
    const pddl_cond_atom_t *a;
    int i;

    BOR_LIST_FOR_EACH(&pddl->init->part, item){
        c = BOR_LIST_ENTRY(item, const pddl_cond_t, conn);
        if (c->type != PDDL_COND_ATOM)
            continue;
        a = PDDL_COND_CAST(c, atom);
        if (a->pred != pred || a->arg_size != arg_size)
            continue;
        for (i = 0; i < arg_size; ++i){
            if (a->arg[i].obj != arg[i])
                break;
        }
        if (i == arg_size)
            return 1;
    }

    return 0;
}

static void addNotPredsToInitRec(pddl_t *pddl, int pos, int neg,
                                 int arg_size, int *arg,
                                 const pddl_pred_t *pred, int argi)
{
    pddl_cond_atom_t *a;
    const int *obj;
    int i, obj_size;

    if (argi == arg_size){
        if (!initHasFact(pddl, pos, arg_size, arg)){
            a = pddlCondCreateFactAtom(neg, arg_size, arg);
            pddlCondPartAdd(pddl->init, &a->cls);
        }

        return;
    }

    obj = pddlTypesObjsByType(&pddl->type, pred->param[argi], &obj_size);
    for (i = 0; i < obj_size; ++i){
        arg[argi] = obj[i];
        addNotPredsToInitRec(pddl, pos, neg, arg_size, arg, pred, argi + 1);
    }
}

static void addNotPredsToInit(pddl_t *pddl, int pos, int neg)
{
    const pddl_pred_t *pos_pred = pddl->pred.pred + pos;
    int arg[pos_pred->param_size];

    // Recursivelly try all possible objects for each argument
    addNotPredsToInitRec(pddl, pos, neg,
                         pos_pred->param_size, arg, pos_pred, 0);
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

static int isFalsePre(const pddl_cond_t *c)
{
    if (c->type == PDDL_COND_BOOL){
        const pddl_cond_bool_t *b = PDDL_COND_CAST(c, bool);
        return !b->val;
    }
    return 0;
}

static void removeIrrelevantActions(pddl_t *pddl)
{
    for (int ai = 0; ai < pddl->action.size;){
        pddl_action_t *a = pddl->action.action + ai;
        a->pre = pddlCondDeconflictPre(a->pre, pddl, &a->param);
        a->eff = pddlCondDeconflictEff(a->eff, pddl, &a->param);

        if (isFalsePre(a->pre) || !pddlCondHasAtom(a->eff)){
            pddlActionFree(a);
            if (ai != pddl->action.size - 1)
                *a = pddl->action.action[pddl->action.size - 1];
            --pddl->action.size;
        }else{
            ++ai;
        }
    }
}


void pddlNormalize(pddl_t *pddl)
{
    int i;

    for (i = 0; i < pddl->action.size; ++i)
        pddlActionNormalize(pddl->action.action + i, pddl);

    for (i = 0; i < pddl->action.size; ++i)
        pddlActionSplit(pddl->action.action + i, &pddl->action);

    removeIrrelevantActions(pddl);

#ifdef PDDL_DEBUG
    for (i = 0; i < pddl->action.size; ++i)
        pddlActionAssertPreConjuction(pddl->action.action + i);
#endif

    if (pddl->goal)
        pddl->goal = pddlCondNormalize(pddl->goal, pddl, NULL);

    compileOutNonStaticNegPre(pddl);
}

void pddlCompileAwayCondEff(pddl_t *pddl)
{
    pddl_action_t *a, *new_a;
    pddl_cond_when_t *w;
    pddl_cond_t *neg_pre;
    int asize;
    int change;

    do {
        change = 0;
        pddlNormalize(pddl);

        asize = pddl->action.size;
        for (int ai = 0; ai < asize; ++ai){
            a = pddl->action.action + ai;
            w = pddlCondRemoveFirstNonStaticWhen(a->eff, pddl);
            if (w != NULL){
                // Create a new action
                new_a = pddlActionsAdd(&pddl->action);

                // Get the original action again, because pddlActionsAdd()
                // could realloc the array.
                a = pddl->action.action + ai;

                // Copy the original to the new
                pddlActionCopy(new_a, a);

                // The original takes additional precondition which is the
                // negation of w->pre
                if ((neg_pre = pddlCondNegatePre(w->pre, pddl)) == NULL){
                    // This shoud never fail, because we force
                    // normalization before this.
                    TRACE;
                    fprintf(stderr, "Fatal Error: Encountered problem in"
                            " the normalization.\n");
                    pddlErrPrint();
                    pddlErrPrintTraceback();
                    exit(-1);
                }
                a->pre = pddlCondNewAnd2(a->pre, neg_pre);

                // The new action extends both pre and eff by w->pre and
                // w->eff.
                new_a->pre = pddlCondNewAnd2(new_a->pre, pddlCondClone(w->pre));
                new_a->eff = pddlCondNewAnd2(new_a->eff, pddlCondClone(w->eff));

                pddlCondDel(&w->cls);
                change = 1;
            }
        }
    } while (change);
}

int pddlPredFuncMaxParamSize(const pddl_t *pddl)
{
    int i, max = 0;

    for (i = 0; i < pddl->pred.size; ++i)
        max = BOR_MAX(max, pddl->pred.pred[i].param_size);
    for (i = 0; i < pddl->func.size; ++i)
        max = BOR_MAX(max, pddl->func.pred[i].param_size);

    return max;
}

void pddlPrintPDDLDomain(const pddl_t *pddl, FILE *fout)
{
    fprintf(fout, "(define (domain %s)\n", pddl->domain_name);
    pddlRequirePrintPDDL(pddl->require, fout);
    pddlTypesPrintPDDL(&pddl->type, fout);
    pddlObjsPrintPDDLConstants(&pddl->obj, &pddl->type, fout);
    pddlPredsPrintPDDL(&pddl->pred, &pddl->type, fout);
    pddlFuncsPrintPDDL(&pddl->func, &pddl->type, fout);
    pddlActionsPrintPDDL(&pddl->action, pddl, fout);
    fprintf(fout, ")\n");
}

void pddlPrintPDDLProblem(const pddl_t *pddl, FILE *fout)
{
    bor_list_t *item;
    pddl_cond_t *c;
    pddl_params_t params;

    fprintf(fout, "(define (problem %s) (:domain %s)\n",
            pddl->problem_name, pddl->domain_name);

    pddlParamsInit(&params);
    fprintf(fout, "(:init\n");
    BOR_LIST_FOR_EACH(&pddl->init->part, item){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        fprintf(fout, " ");
        pddlCondPrintPDDL(c, pddl, &params, fout);
    }
    fprintf(fout, ")\n");
    pddlParamsFree(&params);

    fprintf(fout, "(:goal ");
    pddlCondPrintPDDL(pddl->goal, pddl, NULL, fout);
    fprintf(fout, ")\n");

    if (pddl->metric)
        fprintf(fout, "(:metric minimize (total-cost))\n");

    fprintf(fout, ")\n");
}

static int initCondSize(const pddl_t *pddl, int type)
{
    bor_list_t *item;
    const pddl_cond_t *c;
    int size = 0;

    BOR_LIST_FOR_EACH(&pddl->init->part, item){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        if (c->type == type)
            ++size;
    }
    return size;
}

// TODO: Rename to pddlPrintDebug
void pddlPrintDebug(const pddl_t *pddl, FILE *fout)
{
    bor_list_t *item;
    pddl_cond_t *c;
    pddl_cond_atom_t *a;
    pddl_params_t params;

    fprintf(fout, "Domain: %s\n", pddl->domain_name);
    fprintf(fout, "Problem: %s\n", pddl->problem_name);
    fprintf(fout, "Require: %x\n", pddl->require);
    pddlTypesPrint(&pddl->type, fout);
    pddlObjsPrint(&pddl->obj, fout);
    pddlPredsPrint(&pddl->pred, "Predicate", fout);
    pddlPredsPrint(&pddl->func, "Function", fout);
    pddlActionsPrint(pddl, &pddl->action, fout);

    pddlParamsInit(&params);
    fprintf(fout, "Init[%d]:\n", initCondSize(pddl, PDDL_COND_ATOM));
    BOR_LIST_FOR_EACH(&pddl->init->part, item){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        if (c->type != PDDL_COND_ATOM)
            continue;
        a = PDDL_COND_CAST(c, atom);
        fprintf(fout, "  ");
        if (pddlPredIsStatic(&pddl->pred.pred[a->pred]))
            fprintf(fout, "S:");
        pddlCondPrintPDDL(c, pddl, &params, fout);
        fprintf(fout, "\n");
    }

    fprintf(fout, "Init[%d]:\n", initCondSize(pddl, PDDL_COND_ASSIGN));
    BOR_LIST_FOR_EACH(&pddl->init->part, item){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        if (c->type != PDDL_COND_ASSIGN)
            continue;
        fprintf(fout, "  ");
        pddlCondPrintPDDL(c, pddl, &params, fout);
        fprintf(fout, "\n");
    }
    pddlParamsFree(&params);

    fprintf(fout, "Goal: ");
    pddlCondPrint(pddl, pddl->goal, NULL, fout);
    fprintf(fout, "\n");

    fprintf(fout, "Metric: %d\n", pddl->metric);
}
