/***
 * cpddl
 * -------
 * Copyright (c)2019 Daniel Fiser <danfis@danfis.cz>,
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

#include "pddl/pddl.h"
#include "pddl/lifted_mgroup.h"
#include "assert.h"

void pddlLiftedMGroupInitCandFromPred(pddl_lifted_mgroup_t *mgroup,
                                      const pddl_pred_t *pred,
                                      int counted_var)
{
    pddlParamsInit(&mgroup->param);
    pddlCondArrInit(&mgroup->cond);

    for (int param_id = 0; param_id < pred->param_size; ++param_id){
        pddl_param_t *param = pddlParamsAdd(&mgroup->param);
        param->type = pred->param[param_id];
        if (counted_var == param_id)
            param->is_counted_var = 1;
    }

    pddl_cond_atom_t *atom;
    atom = pddlCondNewEmptyAtom(pred->param_size);
    atom->pred = pred->id;
    for (int param_id = 0; param_id < pred->param_size; ++param_id){
        atom->arg[param_id].param = param_id;
        atom->arg[param_id].obj = PDDL_OBJ_ID_UNDEF;
    }
    pddlCondArrAdd(&mgroup->cond, &atom->cls);
}

void pddlLiftedMGroupFree(pddl_lifted_mgroup_t *mgroup)
{
    for (int i = 0; i < mgroup->cond.size; ++i)
        pddlCondDel((pddl_cond_t *)mgroup->cond.cond[i]);
    pddlParamsFree(&mgroup->param);
    pddlCondArrFree(&mgroup->cond);
}


struct at_most_one {
    pddl_cond_arr_t *atom_arr;
    const pddl_lifted_mgroup_t *cand;
};

static int collectPredMatchingAtoms(pddl_cond_t *c, void *ud)
{
    struct at_most_one *ctx = ud;
    pddl_cond_arr_t *atom_arr = ctx->atom_arr;
    const pddl_lifted_mgroup_t *cand = ctx->cand;

    if (c->type == PDDL_COND_ATOM){
        pddl_cond_atom_t *atom = PDDL_COND_CAST(c, atom);
        for (int i = 0; i < cand->cond.size; ++i){
            const pddl_cond_t *c = cand->cond.cond[i];
            const pddl_cond_atom_t *catom = PDDL_COND_CAST(c, atom);
            if (catom->pred == atom->pred){
                pddlCondArrAdd(atom_arr, &atom->cls);
                return 0;
            }
        }
    }

    return 0;
}

static int canUnifyToGroundedPair(const pddl_lifted_mgroup_t *cand,
                                  const pddl_cond_atom_t *a1,
                                  const pddl_cond_atom_t *a2)
{
    pddl_obj_id_t arg[cand->param.param_size];

    for (int c1i = 0; c1i < cand->cond.size; ++c1i){
        const pddl_cond_t *c1 = cand->cond.cond[c1i];
        const pddl_cond_atom_t *cand1 = PDDL_COND_CAST(c1, atom);
        if (cand1->pred != a1->pred)
            continue;

        // Fix quantified (non-counted) variables
        for (int i = 0; i < cand->param.param_size; ++i)
            arg[i] = PDDL_OBJ_ID_UNDEF;
        for (int i = 0; i < cand1->arg_size; ++i){
            ASSERT(cand1->arg[i].obj == PDDL_OBJ_ID_UNDEF);
            ASSERT(a1->arg[i].obj >= 0);
            int param = cand1->arg[i].param;
            if (!cand->param.param[param].is_counted_var)
                arg[param] = a1->arg[i].obj;
        }

        // and with fixed variables for a1, try to unify a2
        for (int c2i = 0; c2i < cand->cond.size; ++c2i){
            const pddl_cond_t *c2 = cand->cond.cond[c2i];
            const pddl_cond_atom_t *cand2 = PDDL_COND_CAST(c2, atom);
            if (cand2->pred != a2->pred)
                continue;

            int can_unify = 1;
            for (int i = 0; i < cand2->arg_size; ++i){
                ASSERT(cand2->arg[i].obj == PDDL_OBJ_ID_UNDEF);
                ASSERT(a2->arg[i].obj >= 0);
                int param = cand2->arg[i].param;
                if (!cand->param.param[param].is_counted_var
                        && arg[param] != a2->arg[i].obj){
                    can_unify = 0;
                    break;
                }
            }

            if (can_unify)
                return 1;
        }
    }

    return 0;
}

int pddlLiftedMGroupHasAtMostOne(const pddl_lifted_mgroup_t *cand,
                                 const pddl_cond_part_t *grounded_conj)
{
    pddl_cond_arr_t atom_arr;
    struct at_most_one ctx = { &atom_arr, cand };
    int ret = 1;

    pddlCondArrInit(&atom_arr);
    pddlCondTraverse((pddl_cond_t *)&grounded_conj->cls,
                     collectPredMatchingAtoms, NULL, &ctx);

    for (int i = 0; ret == 1 && i < atom_arr.size; ++i){
        const pddl_cond_t *c1 = atom_arr.cond[i];
        const pddl_cond_atom_t *a1 = PDDL_COND_CAST(c1, atom);
        for (int j = i + 1; ret == 1 && j < atom_arr.size; ++j){
            const pddl_cond_t *c2 = atom_arr.cond[j];
            const pddl_cond_atom_t *a2 = PDDL_COND_CAST(c2, atom);
            if (canUnifyToGroundedPair(cand, a1, a2))
                ret = 0;
        }
    }
    pddlCondArrFree(&atom_arr);

    return ret;
}

int pddlLiftedMGroupIsValid(const pddl_lifted_mgroup_t *cand,
                            const pddl_t *pddl)
{
    return 0;
}

int pddlLiftedMGroupIsActionTooHeavy(const pddl_lifted_mgroup_t *mgroup,
                                     const pddl_t *pddl,
                                     int action_id)
{
    return 0;
}

int pddlLiftedMGroupIsActionUnbalanced(const pddl_lifted_mgroup_t *mgroup,
                                       const pddl_t *pddl,
                                       int action_id)
{
    return 0;
}


void pddlLiftedMGroupPrint(const pddl_t *pddl,
                           const pddl_lifted_mgroup_t *mgroup,
                           FILE *fout)
{
    fprintf(fout, "{");

    for (int i = 0; i < mgroup->cond.size; ++i){
        if (i > 0)
            fprintf(fout, ", ");

        pddl_cond_atom_t *atom = PDDL_COND_CAST(mgroup->cond.cond[i], atom);
        fprintf(fout, "%s", pddl->pred.pred[atom->pred].name);
        for (int j = 0; j < atom->arg_size; ++j){
            if (atom->arg[j].param >= 0){
                int param_id = atom->arg[j].param;
                const pddl_param_t *p = mgroup->param.param + param_id;
                if (p->is_counted_var){
                    fprintf(fout, " C");
                }else{
                    fprintf(fout, " V");
                }
            }else{
                fprintf(fout, " %s", pddl->obj.obj[atom->arg[j].obj].name);
            }
        }
    }

    fprintf(fout, "}\n");
}
