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
#include "pddl/strips.h"
#include "err.h"
#include "assert.h"

/** Implemented in strips_ground.c */
int _pddlStripsGround(pddl_strips_t *strips, const pddl_t *pddl);

static void copyBasicInfo(pddl_strips_t *dst, const pddl_strips_t *src)
{
    if (src->domain_name)
        dst->domain_name = BOR_STRDUP(src->domain_name);
    if (src->problem_name)
        dst->problem_name = BOR_STRDUP(src->problem_name);
    if (src->domain_file)
        dst->domain_file = BOR_STRDUP(src->domain_file);
    if (src->problem_file)
        dst->problem_file = BOR_STRDUP(src->problem_file);
}

static pddl_strips_t *stripsNew(void)
{
    pddl_strips_t *strips;

    strips = BOR_ALLOC(pddl_strips_t);
    bzero(strips, sizeof(*strips));
    pddlFactsInit(&strips->fact);
    pddlStripsOpsInit(&strips->op);
    borISetInit(&strips->init);
    borISetInit(&strips->goal);
    pddlMutexesInit(&strips->mutex);
    pddlMGroupsInit(&strips->mgroup);
    return strips;
}

void pddlStripsMakeUnsolvable(pddl_strips_t *strips)
{
    // Remove all operators, empty the initial state and make sure that the
    // goal is non-empty.

    INFO2("The problem is not solvable -- generating a dummy problem.");
    pddlStripsOpsFree(&strips->op);
    pddlStripsOpsInit(&strips->op);
    borISetEmpty(&strips->init);
    if (strips->fact.fact_size == 0){
        // TODO
        FATAL2("STRIPS problem does not contain any fact."
                " Making unsolvable problem for this case is not yet"
                " implemented.");
    }
    borISetEmpty(&strips->goal);
    borISetAdd(&strips->goal, 0);

    ASSERT_RUNTIME(strips->fact.fact_size > 0);
    for (int i = strips->fact.fact_size - 1; i >= 1; --i)
        pddlFactsDelFact(&strips->fact, i);
    strips->fact.fact_size = 1;

    pddlMutexesFree(&strips->mutex);
    pddlMutexesInit(&strips->mutex);

    pddlMGroupsFree(&strips->mgroup);
    pddlMGroupsInit(&strips->mgroup);
}

static void makeExactlyOneExtendGoal(pddl_strips_t *strips,
                                     pddl_mgroup_t *mg,
                                     int none_of_those)
{
    int fact;

    BOR_ISET_FOR_EACH(&mg->fact, fact){
        if (!pddlMutexesIsMutexWithFact(&strips->mutex, fact, &strips->goal))
            return;
    }
    mg->is_goal = 1;
    borISetAdd(&strips->goal, none_of_those);
}

static int makeExactlyOneMGroup(pddl_strips_t *strips,
                                int mgroup_id,
                                pddl_mgroup_t *mg)
{
    pddl_fact_t fact;
    int none_of_those;
    char name[1024], *s;
    BOR_ISET(mutex);
    int fact_id;

    // Create an artificial fact "none-of-those"
    s = name;
    s += sprintf(s, "NOT");
    BOR_ISET_FOR_EACH(&mg->fact, fact_id)
        s += sprintf(s, ":%s", strips->fact.fact[fact_id]->name);
    pddlFactInit(&fact);
    fact.name = BOR_STRDUP(name);
    none_of_those = pddlFactsAdd(&strips->fact, &fact);
    pddlFactFree(&fact);

    // Add none-of-those to init if necessary
    if (borISetIsDisjunct(&strips->init, &mg->fact)){
        borISetAdd(&strips->init, none_of_those);
        mg->is_init = 1;
    }

    // Add none-of-those to the goal if possible
    makeExactlyOneExtendGoal(strips, mg, none_of_those);

    // Add mutexes between none-of-those and all other facts
    BOR_ISET_FOR_EACH(&mg->fact, fact_id){
        borISetEmpty(&mutex);
        borISetAdd(&mutex, fact_id);
        borISetAdd(&mutex, none_of_those);
        pddlMutexesAdd(&strips->mutex, &mutex);
    }

    // Add none-of-those to the mutex group
    borISetAdd(&mg->fact, none_of_those);
    mg->none_of_those = none_of_those;
    mg->is_exactly_1 = 1;

    borISetFree(&mutex);

    return 0;
}

int pddlStripsMakeExactlyOneMGroups(pddl_strips_t *strips)
{
    for (int i = 0; i < strips->mgroup.mgroup_size; ++i){
        pddl_mgroup_t *mg = strips->mgroup.mgroup + i;
        if (!mg->is_exactly_1){
            if (makeExactlyOneMGroup(strips, i, mg) != 0)
                TRACE_RET(-1);
        }
    }
    return 0;
}

void pddlStripsCompleteMGroups(pddl_strips_t *strips)
{
    pddl_mgroup_t *mg;
    BOR_ISET(all);
    BOR_ISET(mgset);
    int fact;

    for (int i = 0; i < strips->fact.fact_size; ++i)
        borISetAdd(&all, i);
    for (int i = 0; i < strips->mgroup.mgroup_size; ++i)
        borISetMinus(&all, &strips->mgroup.mgroup[i].fact);

    BOR_ISET_FOR_EACH(&all, fact){
        borISetEmpty(&mgset);
        borISetAdd(&mgset, fact);
        mg = pddlMGroupsAdd(&strips->mgroup, &mgset);
        if (borISetIn(fact, &strips->init))
            mg->is_init = 1;
        if (borISetIn(fact, &strips->goal))
            mg->is_goal = 1;
    }
    INFO("Added %d single-fact mgroups.", borISetSize(&all));

    borISetFree(&mgset);
    borISetFree(&all);
}

pddl_strips_t *pddlStripsNew(const pddl_t *pddl,
                             const pddl_strips_config_t *cfg)
{
    pddl_strips_t *strips = stripsNew();

    strips->cfg = *cfg;

    if (pddl->domain_name)
        strips->domain_name = BOR_STRDUP(pddl->domain_name);
    if (pddl->problem_name)
        strips->problem_name = BOR_STRDUP(pddl->problem_name);
    if (pddl->domain_lisp->filename)
        strips->domain_file = BOR_STRDUP(pddl->domain_lisp->filename);
    if (pddl->problem_lisp->filename)
        strips->problem_file = BOR_STRDUP(pddl->problem_lisp->filename);

    if (_pddlStripsGround(strips, pddl) != 0){
        pddlStripsDel(strips);
        TRACE_RET(NULL);
    }

    // TODO: remove identical/dominated operators
    //       (don't forget to keep the one with the minimal cost)

    if (strips->cfg.prune.enable){
        if (pddlStripsPrune(strips, &strips->cfg.prune) != 0){
            pddlStripsDel(strips);
            TRACE_RET(NULL);
        }
    }

    if (strips->goal_is_unreachable){
        pddlStripsMakeUnsolvable(strips);
        return strips;
    }

    // Infer fa mutex groups only if they were not already infered during
    // pruning.
    if (strips->cfg.fa_mgroup &&
            (!strips->cfg.prune.enable || !strips->cfg.prune.fa_mgroup)){
        if (pddlMGroupsFA(strips, &strips->mgroup) != 0){
            pddlStripsDel(strips);
            TRACE_RET(NULL);
        }
        pddlMGroupsFinalize(&strips->mgroup, strips);
        INFO2("Fact-alternating mutex groups are infered.");
    }

    if (strips->cfg.h_mutex > 0){
        if (!strips->cfg.prune.enable
                || strips->cfg.prune.h_mutex < strips->cfg.h_mutex){
            if (pddlMutexesHm(&strips->mutex, strips->cfg.h_mutex, strips,
                              NULL, 0, -1.) != 0){
                pddlStripsDel(strips);
                TRACE_RET(NULL);
            }
            INFO("h^%d mutexes are infered.", strips->cfg.h_mutex);
        }else{
            pddlMutexesHmLimit(&strips->mutex, strips->cfg.h_mutex);
        }
    }

    return strips;
}

void pddlStripsDel(pddl_strips_t *strips)
{
    if (strips->domain_name)
        BOR_FREE(strips->domain_name);
    if (strips->problem_name)
        BOR_FREE(strips->problem_name);
    if (strips->domain_file)
        BOR_FREE(strips->domain_file);
    if (strips->problem_file)
        BOR_FREE(strips->problem_file);
    pddlFactsFree(&strips->fact);
    pddlStripsOpsFree(&strips->op);
    borISetFree(&strips->init);
    borISetFree(&strips->goal);
    pddlMutexesFree(&strips->mutex);
    pddlMGroupsFree(&strips->mgroup);
    BOR_FREE(strips);
}

pddl_strips_t *pddlStripsClone(const pddl_strips_t *src)
{
    pddl_strips_t *dst;
    dst = BOR_ALLOC(pddl_strips_t);

    bzero(dst, sizeof(*dst));
    dst->cfg = src->cfg;
    if (src->domain_name != NULL)
        dst->domain_name = BOR_STRDUP(src->domain_name);
    if (src->problem_name != NULL)
        dst->problem_name = BOR_STRDUP(src->problem_name);
    if (src->domain_file != NULL)
        dst->domain_file = BOR_STRDUP(src->domain_file);
    if (src->problem_file != NULL)
        dst->problem_file = BOR_STRDUP(src->problem_file);
    pddlFactsInit(&dst->fact);
    pddlStripsOpsInit(&dst->op);

    pddlFactsCopy(&dst->fact, &src->fact);
    pddlStripsOpsCopy(&dst->op, &src->op);
    borISetUnion(&dst->init, &src->init);
    borISetUnion(&dst->goal, &src->goal);
    pddlMutexesCopy(&dst->mutex, &src->mutex);
    pddlMGroupsCopy(&dst->mgroup, &src->mgroup);
    dst->goal_is_unreachable = src->goal_is_unreachable;
    dst->has_cond_eff = src->has_cond_eff;

    return dst;
}

pddl_strips_t *pddlStripsDual(const pddl_strips_t *strips)
{
    pddl_strips_t *dual = stripsNew();
    pddl_strips_op_t op;

    copyBasicInfo(dual, strips);
    pddlFactsCopy(&dual->fact, &strips->fact);

    // Construct initial state and goal specification
    for (int i = 0; i < dual->fact.fact_size; ++i){
        borISetAdd(&dual->init, i);
        borISetAdd(&dual->goal, i);
    }
    borISetMinus(&dual->init, &strips->goal);
    borISetMinus(&dual->goal, &strips->init);

    // Copy dual operators
    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *sop = strips->op.op[i];
        pddlStripsOpInit(&op);
        pddlStripsOpCopyDual(&op, sop);
        pddlStripsOpsAdd(&dual->op, &op);
        pddlStripsOpFree(&op);
    }

    dual->goal_is_unreachable = strips->goal_is_unreachable;
    dual->has_cond_eff = strips->has_cond_eff;

    return dual;
}

pddl_strips_t *pddlStripsRelaxedBackward(const pddl_strips_t *strips)
{
    pddl_strips_t *bw = stripsNew();
    pddl_strips_op_t op;
    BOR_ISET(fset);
    int fact;

    copyBasicInfo(bw, strips);
    pddlFactsCopy(&bw->fact, &strips->fact);

    // Keep goal specification and delete effects empty.

    // Construct initial state as all facts minus the unreachable ones and
    // minus mutexes with strips->goal.
    for (int f = 0; f < bw->fact.fact_size; ++f){
        BOR_ISET_SET(&fset, f);
        if (pddlMutexesIsMutex(&strips->mutex, &fset))
            continue;
        if (!pddlMutexesIsMutexWithFact(&strips->mutex, f, &strips->goal))
            borISetAdd(&bw->init, f);
    }

    // Create operators
    for (int opi = 0; opi < strips->op.op_size; ++opi){
        const pddl_strips_op_t *sop = strips->op.op[opi];
        pddlStripsOpInit(&op);
        pddlStripsOpCopy(&op, sop);

        // Set precondition as prevail + add effect from sop
        borISetMinus2(&fset, &sop->pre, &sop->del_eff);
        borISetUnion2(&op.pre, &fset, &sop->add_eff);

        // Set add effects as delete effects
        borISetSet(&op.add_eff, &sop->del_eff);

        // Set delete effect as facts that are e-deleted by prevails and
        // and add effects
        borISetMinus2(&fset, &sop->pre, &sop->del_eff);
        borISetUnion(&fset, &op.add_eff);
        borISetEmpty(&op.del_eff);
        BOR_ISET_FOR_EACH(&fset, fact){
            for (int f = 0; f < strips->fact.fact_size; ++f){
                if (pddlMutexesIsMutexBetweenTwoFacts(&strips->mutex, fact, f))
                    borISetAdd(&op.del_eff, f);
            }
        }

        pddlStripsOpNormalize(&op);
        pddlStripsOpsAdd(&bw->op, &op);
        pddlStripsOpFree(&op);
    }

    return bw;
}

pddl_strips_t *pddlStripsCompileAwayCondEffRelaxed(const pddl_strips_t *strips)
{
    pddl_strips_t *s = stripsNew();
    pddl_strips_op_t op;

    copyBasicInfo(s, strips);
    pddlFactsCopy(&s->fact, &strips->fact);
    borISetUnion(&s->init, &strips->init);
    borISetUnion(&s->goal, &strips->goal);

    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *sop = strips->op.op[i];
        pddlStripsOpInit(&op);
        pddlStripsOpCopyWithoutCondEff(&op, sop);
        pddlStripsOpNormalize(&op);
        pddlStripsOpsAdd(&s->op, &op);
        pddlStripsOpFree(&op);
        for (int cei = 0; cei < sop->cond_eff_size; ++cei){
            const pddl_strips_op_cond_eff_t *ce = sop->cond_eff + cei;
            pddlStripsOpInit(&op);
            pddlStripsOpCopyWithoutCondEff(&op, sop);
            borISetUnion(&op.pre, &ce->pre);
            borISetUnion(&op.del_eff, &ce->del_eff);
            borISetUnion(&op.add_eff, &ce->add_eff);
            pddlStripsOpNormalize(&op);
            pddlStripsOpsAdd(&s->op, &op);
            pddlStripsOpFree(&op);
        }
    }

    s->goal_is_unreachable = strips->goal_is_unreachable;

    return s;
}

void pddlStripsCrossRefFactsOps(const pddl_strips_t *strips,
                                void *_fact_arr,
                                unsigned long el_size,
                                long pre_offset,
                                long add_offset,
                                long del_offset)
{
    char *fact_arr = _fact_arr;
    for (int op_id = 0; op_id < strips->op.op_size; ++op_id){
        const pddl_strips_op_t *op = strips->op.op[op_id];
        int fact_id;

        if (pre_offset >= 0){
            BOR_ISET_FOR_EACH(&op->pre, fact_id){
                char *el = fact_arr + (el_size * fact_id);
                bor_iset_t *s = (bor_iset_t *)(el + pre_offset);
                borISetAdd(s, op_id);
            }
        }

        if (add_offset >= 0){
            BOR_ISET_FOR_EACH(&op->add_eff, fact_id){
                char *el = fact_arr + (el_size * fact_id);
                bor_iset_t *s = (bor_iset_t *)(el + add_offset);
                borISetAdd(s, op_id);
            }
        }

        if (del_offset >= 0){
            BOR_ISET_FOR_EACH(&op->del_eff, fact_id){
                char *el = fact_arr + (el_size * fact_id);
                bor_iset_t *s = (bor_iset_t *)(el + del_offset);
                borISetAdd(s, op_id);
            }
        }
    }
}

void pddlStripsApplicableOps(const pddl_strips_t *strips,
                             const bor_iset_t *state,
                             bor_iset_t *app_ops)
{
    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        if (borISetIsSubset(&op->pre, state))
            borISetAdd(app_ops, i);
    }
}


static void printPythonISet(const bor_iset_t *s, FILE *fout)
{
    int i;
    fprintf(fout, "set([");
    BOR_ISET_FOR_EACH(s, i)
        fprintf(fout, " %d,", i);
    fprintf(fout, "])");
}

void pddlStripsPrintPython(const pddl_strips_t *strips, FILE *fout)
{
    int f;

    fprintf(fout, "{\n");
    fprintf(fout, "'domain_file' : '%s',\n", strips->domain_file);
    fprintf(fout, "'problem_file' : '%s',\n", strips->problem_file);
    fprintf(fout, "'domain_name' : '%s',\n", strips->domain_name);
    fprintf(fout, "'problem_name' : '%s',\n", strips->problem_name);

    fprintf(fout, "'fact' : [\n");
    for (int i = 0; i < strips->fact.fact_size; ++i)
        fprintf(fout, "    '(%s)',\n", strips->fact.fact[i]->name);
    fprintf(fout, "],\n");

    fprintf(fout, "'op' : [\n");
    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        fprintf(fout, "    {\n");
        fprintf(fout, "        'name' : '%s',\n", op->name);
        fprintf(fout, "        'cost' : '%d',\n", op->cost);

        fprintf(fout, "        'pre' : ");
        printPythonISet(&op->pre, fout);
        fprintf(fout, ",\n");
        fprintf(fout, "        'add' : ");
        printPythonISet(&op->add_eff, fout);
        fprintf(fout, ",\n");
        fprintf(fout, "        'del' : ");
        printPythonISet(&op->del_eff, fout);
        fprintf(fout, ",\n");

        fprintf(fout, "        'cond_eff' : [\n");
        for (int j = 0; j < op->cond_eff_size; ++j){
            const pddl_strips_op_cond_eff_t *ce = op->cond_eff + j;
            fprintf(fout, "            {\n");
            fprintf(fout, "                'pre' : ");
            printPythonISet(&ce->pre, fout);
            fprintf(fout, ",\n");
            fprintf(fout, "                'add' : ");
            printPythonISet(&ce->add_eff, fout);
            fprintf(fout, ",\n");
            fprintf(fout, "                'del' : ");
            printPythonISet(&ce->del_eff, fout);
            fprintf(fout, ",\n");
            fprintf(fout, "            },\n");
        }
        fprintf(fout, "        ]\n");

        fprintf(fout, "    },\n");
    }
    fprintf(fout, "],\n");

    fprintf(fout, "'init' : [");
    BOR_ISET_FOR_EACH(&strips->init, f)
        fprintf(fout, "%d, ", f);
    fprintf(fout, "],\n");

    fprintf(fout, "'goal' : [");
    BOR_ISET_FOR_EACH(&strips->goal, f)
        fprintf(fout, "%d, ", f);
    fprintf(fout, "],\n");

    fprintf(fout, "'mgroup' : ");
    pddlMGroupsPrintPython(&strips->mgroup, fout);
    fprintf(fout, ",\n");

    fprintf(fout, "'mutex' : ");
    pddlMutexesPrintPython(&strips->mutex, fout);
    fprintf(fout, ",\n");

    fprintf(fout, "'goal_is_unreachable' : %s,\n",
            (strips->goal_is_unreachable ? "True" : "False" ));
    fprintf(fout, "'has_cond_eff' : %s,\n",
            (strips->has_cond_eff ? "True" : "False" ));
    fprintf(fout, "}\n");
}

void pddlStripsPrintPDDLDomain(const pddl_strips_t *strips, FILE *fout)
{
    int fact_id;

    fprintf(fout, "(define (domain %s)\n", strips->domain_name);

    fprintf(fout, "(:predicates\n");
    for (int i = 0; i < strips->fact.fact_size; ++i)
        fprintf(fout, "    (F%d) ;; %s\n", i, strips->fact.fact[i]->name);
    fprintf(fout, ")\n");
    fprintf(fout, "(:functions (total-cost))\n");

    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        char *name = BOR_STRDUP(op->name);
        for (char *c = name; *c != 0x0; ++c){
            if (*c == ' ' || *c == '(' || *c == ')')
                *c = '_';
        }
        fprintf(fout, "(:action %s\n", name);
        fprintf(fout, "    :precondition (and");
        BOR_ISET_FOR_EACH(&op->pre, fact_id)
            fprintf(fout, " (F%d)", fact_id);
        fprintf(fout, ")\n");

        fprintf(fout, "    :effect (and");
        BOR_ISET_FOR_EACH(&op->add_eff, fact_id)
            fprintf(fout, " (F%d)", fact_id);
        BOR_ISET_FOR_EACH(&op->del_eff, fact_id)
            fprintf(fout, " (not (F%d))", fact_id);
        for (int cei = 0; cei < op->cond_eff_size; ++cei){
            const pddl_strips_op_cond_eff_t *ce = op->cond_eff + cei;
            fprintf(fout, " (when (and");
            BOR_ISET_FOR_EACH(&ce->pre, fact_id)
                fprintf(fout, " (F%d)", fact_id);
            fprintf(fout, ") (and");
            BOR_ISET_FOR_EACH(&ce->add_eff, fact_id)
                fprintf(fout, " (F%d)", fact_id);
            BOR_ISET_FOR_EACH(&ce->del_eff, fact_id)
                fprintf(fout, " (not (F%d))", fact_id);
            fprintf(fout, ")");
        }

        fprintf(fout, " (increase (total-cost) %d)", op->cost);
        fprintf(fout, ")\n");

        fprintf(fout, ")\n");
        BOR_FREE(name);
    }

    fprintf(fout, ")\n");
}

void pddlStripsPrintPDDLProblem(const pddl_strips_t *strips, FILE *fout)
{
    int fact_id;

    fprintf(fout, "(define (problem %s) (:domain %s)\n",
            strips->problem_name, strips->domain_name);

    fprintf(fout, "(:init\n");
    BOR_ISET_FOR_EACH(&strips->init, fact_id)
        fprintf(fout, "    (F%d)\n", fact_id);
    fprintf(fout, ")\n");

    fprintf(fout, "(:goal (and");
    BOR_ISET_FOR_EACH(&strips->goal, fact_id)
        fprintf(fout, " (F%d)", fact_id);
    fprintf(fout, "))\n");
    fprintf(fout, "(:metric minimize (total-cost))\n");
    fprintf(fout, ")\n");
}

void pddlStripsPrintDebug(const pddl_strips_t *strips, FILE *fout)
{
    fprintf(fout, "Fact[%d]:\n", strips->fact.fact_size);
    pddlFactsPrintSorted(&strips->fact, "  (", ")\n", fout);

    fprintf(fout, "Op[%d]:\n", strips->op.op_size);
    pddlStripsOpsPrintDebug(&strips->op, &strips->fact, fout);

    fprintf(fout, "Init State:");
    pddlFactsIdSetPrintSorted(&strips->init, &strips->fact, " (", ")", fout);
    fprintf(fout, "\n");

    fprintf(fout, "Goal:");
    pddlFactsIdSetPrintSorted(&strips->goal, &strips->fact, " (", ")", fout);
    fprintf(fout, "\n");
    if (strips->goal_is_unreachable)
        fprintf(fout, "Goal is unreachable\n");
    if (strips->has_cond_eff)
        fprintf(fout, "Has conditional effects\n");
}
