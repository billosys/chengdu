/***
 * cpddl
 * -------
 * Copyright (c)2019 Daniel Fiser <danfis@danfis.cz>,
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

#include "pddl/fdr.h"
#include "assert.h"

static void stripsToFDRState(const pddl_fdr_vars_t *fdr_var,
                             const bor_iset_t *state,
                             int *fdr_state)
{
    for (int vi = 0; vi < fdr_var->var_size; ++vi)
        fdr_state[vi] = -1;

    int fact_id;
    BOR_ISET_FOR_EACH(state, fact_id){
        const pddl_fdr_val_t *v = fdr_var->strips_id_to_val[fact_id];
        fdr_state[v->var_id] = v->val_id;
    }

    for (int vi = 0; vi < fdr_var->var_size; ++vi){
        if (fdr_state[vi] == -1){
            ASSERT(fdr_var->var[vi].val_none_of_those >= 0);
            fdr_state[vi] = fdr_var->var[vi].val_none_of_those;
        }
    }
}


static void stripsToFDREff(const pddl_mutex_pairs_t *mutex,
                           const pddl_fdr_vars_t *fdr_var,
                           const bor_iset_t *pre,
                           const bor_iset_t *add_eff,
                           const bor_iset_t *del_eff,
                           int *fdr_pre,
                           int *fdr_eff)
{
    int fact_id;

    for (int vi = 0; vi < fdr_var->var_size; ++vi)
        fdr_pre[vi] = fdr_eff[vi] = -1;

    BOR_ISET_FOR_EACH(del_eff, fact_id){
        const pddl_fdr_val_t *v = fdr_var->strips_id_to_val[fact_id];
        const pddl_fdr_var_t *var = fdr_var->var + v->var_id;
        if (borISetIn(fact_id, pre))
            fdr_pre[var->var_id] = v->val_id;
        if (borISetIn(fact_id, pre)
                || !pddlMutexPairsIsMutexFactSet(mutex, fact_id, pre)){
            if (var->val_none_of_those >= 0)
                fdr_eff[var->var_id] = var->val_none_of_those;
        }
    }

    BOR_ISET_FOR_EACH(add_eff, fact_id){
        const pddl_fdr_val_t *v = fdr_var->strips_id_to_val[fact_id];
        fdr_eff[v->var_id] = v->val_id;
    }
}

struct fdr_eff {
    bor_iset_t pre;
    int *eff_pre;
    int *eff_eff;
};
typedef struct fdr_eff fdr_eff_t;

static void fdOp(const pddl_strips_t *strips,
                 const pddl_mutex_pairs_t *mutex,
                 const pddl_fdr_vars_t *fdr_var,
                 const pddl_strips_op_t *op,
                 FILE *fout)
{
    int fact_id;

    fprintf(fout, "begin_operator\n");
    fprintf(fout, "%s\n", op->name);

    BOR_ISET(prevail);
    borISetMinus2(&prevail, &op->pre, &op->del_eff);
    fprintf(fout, "%d\n", borISetSize(&prevail));
    BOR_ISET_FOR_EACH(&prevail, fact_id){
        const pddl_fdr_val_t *v = fdr_var->strips_id_to_val[fact_id];
        fprintf(fout, "%d %d\n", v->var_id, v->val_id);
    }
    borISetFree(&prevail);

    int num_effs = 0;
    fdr_eff_t *eff;
    eff = BOR_CALLOC_ARR(fdr_eff_t, 1 + op->cond_eff_size);
    for (int i = 0; i < 1 + op->cond_eff_size; ++i){
        borISetFree(&eff[i].pre);
        eff[i].eff_pre = BOR_ALLOC_ARR(int, fdr_var->var_size);
        eff[i].eff_eff = BOR_ALLOC_ARR(int, fdr_var->var_size);
        if (i == 0){
            stripsToFDREff(mutex, fdr_var,
                           &op->pre, &op->add_eff, &op->del_eff,
                           eff[i].eff_pre, eff[i].eff_eff);
        }else{
            const pddl_strips_op_cond_eff_t *ce = op->cond_eff + i - 1;
            borISetUnion(&eff[i].pre, &ce->pre);
            stripsToFDREff(mutex, fdr_var,
                           &ce->pre, &ce->add_eff, &ce->del_eff,
                           eff[i].eff_pre, eff[i].eff_eff);
        }

        num_effs = 0;
        for (int vi = 0; vi < fdr_var->var_size; ++vi){
            if (eff[i].eff_eff[vi] >= 0)
                ++num_effs;
        }
    }

    fprintf(fout, "%d\n", num_effs);
    for (int i = 0; i < 1 + op->cond_eff_size; ++i){
        for (int vi = 0; vi < fdr_var->var_size; ++vi){
            if (eff[i].eff_eff[vi] < 0)
                continue;
            fprintf(fout, "%d", borISetSize(&eff[i].pre));
            BOR_ISET_FOR_EACH(&eff[i].pre, fact_id){
                const pddl_fdr_val_t *v = fdr_var->strips_id_to_val[fact_id];
                fprintf(fout, " %d %d", v->var_id, v->val_id);
            }
            fprintf(fout, " %d %d %d\n",
                    vi, eff[i].eff_pre[vi], eff[i].eff_eff[vi]);
        }
    }


    for (int i = 0; i < 1 + op->cond_eff_size; ++i){
        borISetFree(&eff[i].pre);
        if (eff[i].eff_pre != NULL)
            BOR_FREE(eff[i].eff_pre);
        if (eff[i].eff_eff != NULL)
            BOR_FREE(eff[i].eff_eff);
    }
    if (eff != NULL)
        BOR_FREE(eff);

    fprintf(fout, "%d\n", op->cost);
    fprintf(fout, "end_operator\n");
}

void pddlFDRPrintAsFD(const pddl_strips_t *strips,
                      const pddl_mgroups_t *mg,
                      const pddl_mutex_pairs_t *mutex,
                      unsigned fdr_var_flags,
                      FILE *fout,
                      bor_err_t *err)
{
    pddl_fdr_vars_t fdr_var;

    if (pddlFDRVarsInitFromStrips(&fdr_var, strips, mg, mutex,
                                  fdr_var_flags) != 0){
        return;
    }

    BOR_INFO(err, "Created %d variables.", fdr_var.var_size);
    int num_none_of_those = 0;
    for (int vi = 0; vi < fdr_var.var_size; ++vi){
        if (fdr_var.var[vi].val_none_of_those != -1)
            ++num_none_of_those;
    }
    BOR_INFO(err, "Created %d none-of-those values.", num_none_of_those);

    fprintf(fout, "begin_version\n3\nend_version\n");
    fprintf(fout, "begin_metric\n1\nend_metric\n");

    // variables
    fprintf(fout, "%d\n", fdr_var.var_size);
    for (int vi = 0; vi < fdr_var.var_size; ++vi){
        const pddl_fdr_var_t *var = fdr_var.var + vi;
        fprintf(fout, "begin_variable\n");
        fprintf(fout, "var%d\n", vi);
        fprintf(fout, "-1\n");
        fprintf(fout, "%d\n", var->val_size);
        for (int vali = 0; vali < var->val_size; ++vali)
            fprintf(fout, "%s\n", var->val[vali].name);
        fprintf(fout, "end_variable\n");
    }

    // mutex groups
    fprintf(fout, "%d\n", mg->mgroup_size);
    for (int mi = 0; mi < mg->mgroup_size; ++mi){
        const pddl_mgroup_t *m = mg->mgroup + mi;
        fprintf(fout, "begin_mutex_group\n");
        fprintf(fout, "%d\n", borISetSize(&m->mgroup));
        int fact_id;
        BOR_ISET_FOR_EACH(&m->mgroup, fact_id){
            const pddl_fdr_val_t *v = fdr_var.strips_id_to_val[fact_id];
            fprintf(fout, "%d %d\n", v->var_id, v->val_id);
        }
        fprintf(fout, "end_mutex_group\n");
    }

    // initial state
    fprintf(fout, "begin_state\n");
    int *init = BOR_ALLOC_ARR(int, fdr_var.var_size);
    stripsToFDRState(&fdr_var, &strips->init, init);
    for (int vi = 0; vi < fdr_var.var_size; ++vi)
        fprintf(fout, "%d\n", init[vi]);
    if (init != NULL)
        BOR_FREE(init);
    fprintf(fout, "end_state\n");

    // goal
    fprintf(fout, "begin_goal\n");
    fprintf(fout, "%d\n", borISetSize(&strips->goal));
    int fact_id;
    BOR_ISET_FOR_EACH(&strips->goal, fact_id){
        const pddl_fdr_val_t *v = fdr_var.strips_id_to_val[fact_id];
        fprintf(fout, "%d %d\n", v->var_id, v->val_id);
    }
    fprintf(fout, "end_goal\n");

    // operators
    fprintf(fout, "%d\n", strips->op.op_size);
    for (int op_id = 0; op_id < strips->op.op_size; ++op_id)
        fdOp(strips, mutex, &fdr_var, strips->op.op[op_id], fout);

    // axioms
    fprintf(fout, "0\n");

    pddlFDRVarsFree(&fdr_var);
}
