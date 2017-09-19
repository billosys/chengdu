/***
 * cpddl
 * -------
 * Copyright (c)2017 Daniel Fiser <danfis@danfis.cz>,
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
#include <boruvka/sort.h>
#include <boruvka/lp.h>
#include "pddl/mgroup.h"
#include "pddl/strips.h"
#include "err.h"

void pddlMGroupInit(pddl_mgroup_t *mg)
{
    bzero(mg, sizeof(*mg));
    borISetInit(&mg->fact);
}

void pddlMGroupFree(pddl_mgroup_t *mg)
{
    borISetFree(&mg->fact);
}

void pddlMGroupsInit(pddl_mgroups_t *mgs)
{
    bzero(mgs, sizeof(*mgs));
}

void pddlMGroupsFree(pddl_mgroups_t *mgs)
{
    for (int i = 0; i < mgs->size; ++i)
        pddlMGroupFree(mgs->g + i);
    if (mgs->g != NULL)
        BOR_FREE(mgs->g);
}

pddl_mgroups_t *pddlMGroupsNew(void)
{
    pddl_mgroups_t *mgs;
    mgs = BOR_ALLOC(pddl_mgroups_t);
    pddlMGroupsInit(mgs);
    return mgs;
}

void pddlMGroupsDel(pddl_mgroups_t *mgs)
{
    pddlMGroupsFree(mgs);
    BOR_FREE(mgs);
}

pddl_mgroup_t *pddlMGroupsAdd(pddl_mgroups_t *mgs, const bor_iset_t *mg)
{
    pddl_mgroup_t *g;

    if (mgs->size >= mgs->alloc){
        if (mgs->alloc == 0)
            mgs->alloc = 1;
        mgs->alloc *= 2;
        mgs->g = BOR_REALLOC_ARR(mgs->g, pddl_mgroup_t, mgs->alloc);
    }

    g = mgs->g + mgs->size++;
    pddlMGroupInit(g);
    borISetUnion(&g->fact, mg);
    return g;
}


static int faIsExactly1(const pddl_strips_t *strips,
                        const pddl_mgroup_t *mg)
{
    for (int oi = 0; oi < strips->op.op_size; ++oi){
        const pddl_strips_op_t *op = strips->op.op[oi];
        if (!borISetIntersectionSizeAtLeast(&mg->fact, &op->add_eff, 1)
                && borISetIntersectionSizeAtLeast(&mg->fact, &op->del_eff, 1)){
            return 0;
        }
    }

    return 1;
}

int pddlMGroupsFA(const pddl_strips_t *strips, pddl_mgroups_t *mgs)
{
    pddl_mgroup_t *mg;
    bor_lp_t *lp;
    unsigned lp_flags;
    bor_iset_t predel, fa_mgroup;
    int rows, fact;
    double val, *obj;

    if (!borLPSolverAvailable(BOR_LP_DEFAULT)){
        ERR_RET2(-1, "Cannot compute fam-groups, because ILP solver is not"
                     " avaiable.");
    }

    if (strips->has_cond_eff){
        ERR_RET2(-1, "Cannot compute fam-groups on problems with conditional"
                     " effects. (They can be compiled away.)");
    }

    lp_flags  = BOR_LP_DEFAULT;
    lp_flags |= BOR_LP_NUM_THREADS(1); // TODO: Parametrize
    lp_flags |= BOR_LP_MAX;
    rows = strips->op.op_size + 1;
    lp = borLPNew(rows, strips->fact.fact_size, lp_flags);

    // Set up coeficients in the objective function and set up binary
    // variables
    for (int i = 0; i < strips->fact.fact_size; ++i){
        borLPSetObj(lp, i, 1.);
        borLPSetVarBinary(lp, i);
    }

    // Initial state constraintf
    BOR_ISET_FOR_EACH(&strips->init, fact)
        borLPSetCoef(lp, 0, fact, 1.);
    borLPSetRHS(lp, 0, 1., 'L');

    // Operator constraints
    borISetInit(&predel);
    for (int oi = 0; oi < strips->op.op_size; ++oi){
        const pddl_strips_op_t *op = strips->op.op[oi];
        BOR_ISET_FOR_EACH(&op->add_eff, fact)
            borLPSetCoef(lp, oi + 1, fact, 1.);

        borISetEmpty(&predel);
        borISetUnion(&predel, &op->pre);
        borISetIntersect(&predel, &op->del_eff);
        BOR_ISET_FOR_EACH(&predel, fact)
            borLPSetCoef(lp, oi + 1, fact, -1.);
        borLPSetRHS(lp, oi + 1, 0., 'L');
    }
    borISetFree(&predel);

    borISetInit(&fa_mgroup);
    obj = BOR_ALLOC_ARR(double, strips->fact.fact_size);
    while (borLPSolve(lp, &val, obj) == 0 && val > 0.5){
        double rhs = 1.;
        char sense = 'G';
        borLPAddRows(lp, 1, &rhs, &sense);
        borISetEmpty(&fa_mgroup);
        for (int i = 0; i < strips->fact.fact_size; ++i){
            if (obj[i] < 0.5){
                borLPSetCoef(lp, rows, i, 1.);
            }else{
                borISetAdd(&fa_mgroup, i);
            }
        }
        mg = pddlMGroupsAdd(mgs, &fa_mgroup);
        mg->is_fa = 1;
        if (borISetIntersectionSizeAtLeast(&mg->fact, &strips->init, 1))
            mg->is_init = 1;
        if (borISetIntersectionSizeAtLeast(&mg->fact, &strips->goal, 1))
            mg->is_goal = mg->is_exactly_1 = 1;
        // TODO: parametrize
        if (!mg->is_goal && faIsExactly1(strips, mg))
            mg->is_exactly_1 = 1;
        ++rows;
    }
    BOR_FREE(obj);
    borISetFree(&fa_mgroup);

    borLPDel(lp);

    return 0;
}

pddl_mgroups_t *pddlMGroupsFANew(const pddl_strips_t *strips)
{
    pddl_mgroups_t *mgs = pddlMGroupsNew();
    if (pddlMGroupsFA(strips, mgs) != 0){
        pddlMGroupsDel(mgs);
        TRACE_RET(NULL);
    }
    return mgs;
}


static int prettyMutexCmp(const void *a, const void *b, void *_fs)
{
    const pddl_facts_t *fs = _fs;
    int *m1 = *(int **)a;
    int *m2 = *(int **)b;
    if (m1[0] != m2[0])
        return m1[0] - m2[0];
    for (int i = 0; i < m1[0]; ++i){
        const pddl_fact_t *f1 = fs->fact[m1[i + 1]];
        const pddl_fact_t *f2 = fs->fact[m2[i + 1]];
        int cmp = pddlFactCmp(f1, f2);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

static int prettyFactCmp(const void *a, const void *b, void *_fs)
{
    const pddl_facts_t *fs = _fs;
    int fid1 = *(int *)a;
    int fid2 = *(int *)b;
    const pddl_fact_t *f1 = fs->fact[fid1];
    const pddl_fact_t *f2 = fs->fact[fid2];
    return pddlFactCmp(f1, f2);
}

struct pretty {
    int **m;
    int size;
};
typedef struct pretty pretty_t;

void pddlMGroupsPrettyPrint(const struct pddl *pddl, const pddl_facts_t *fs,
                            const pddl_mgroups_t *ms, FILE *fout)
{
    pretty_t p;

    if (ms->size == 0)
        return;

    p.size = ms->size;
    p.m = BOR_ALLOC_ARR(int *, ms->size);
    for (int i = 0; i < ms->size; ++i){
        p.m[i] = BOR_ALLOC_ARR(int, ms->g[i].fact.size + 1);
        p.m[i][0] = ms->g[i].fact.size;
        for (int j = 0; j < ms->g[i].fact.size; ++j)
            p.m[i][j + 1] = borISetGet(&ms->g[i].fact, j);
        borSort(p.m[i] + 1, ms->g[i].fact.size, sizeof(int),
                prettyFactCmp, (void *)fs);
    }
    borSort(p.m, p.size, sizeof(int *), prettyMutexCmp, (void *)fs);

    for (int i = 0; i < p.size; ++i){
        if (ms->g[i].is_init)
            fprintf(fout, "i:");
        if (ms->g[i].is_goal)
            fprintf(fout, "g:");
        if (ms->g[i].is_fa)
            fprintf(fout, "fa:");
        if (ms->g[i].is_exactly_1)
            fprintf(fout, "e1:");
        fprintf(fout, "%d :: ", p.m[i][0]);
        for (int j = 0; j < p.m[i][0]; ++j){
            if (j > 0)
                fprintf(fout, "; ");
            fprintf(fout, "%s", fs->fact[p.m[i][j + 1]]->name);
        }
        fprintf(fout, "\n");
    }

    for (int i = 0; i < ms->size; ++i)
        BOR_FREE(p.m[i]);
    BOR_FREE(p.m);
}

void pddlMGroupsPrintPython(const pddl_mgroups_t *mg, FILE *fout)
{
    fprintf(fout, "[\n");
    for (int i = 0; i < mg->size; ++i){
        const pddl_mgroup_t *g = mg->g + i;
        int fact_id;

        fprintf(fout, "    {\n");

        fprintf(fout, "        'fact' : set([");
        BOR_ISET_FOR_EACH(&g->fact, fact_id)
            fprintf(fout, " %d,", fact_id);
        fprintf(fout, "]),\n");
        fprintf(fout, "        'is_init' : %s,\n",
                (g->is_init ? "True" : "False"));
        fprintf(fout, "        'is_goal' : %s,\n",
                (g->is_goal ? "True" : "False"));
        fprintf(fout, "        'is_fa' : %s,\n",
                (g->is_fa ? "True" : "False"));
        fprintf(fout, "        'is_exactly_1' : %s,\n",
                (g->is_exactly_1 ? "True" : "False"));

        fprintf(fout, "    },\n");
    }
    fprintf(fout, "]\n");
}
