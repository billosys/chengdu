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

static void collectCandAtomsFromAnd(const pddl_lifted_mgroup_t *cand,
                                    const pddl_cond_part_t *and,
                                    pddl_cond_arr_t *atoms,
                                    int positive,
                                    int negative)
{
    bor_list_t *item;
    const pddl_cond_t *c;

    BOR_LIST_FOR_EACH(&and->part, item){
        c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
        if (c->type == PDDL_COND_ATOM){
            const pddl_cond_atom_t *a = PDDL_COND_CAST(c, atom);
            if ((a->neg && !negative) || (!a->neg && !positive))
                continue;

            for (int i = 0; i < cand->cond.size; ++i){
                const pddl_cond_t *cc = cand->cond.cond[i];
                const pddl_cond_atom_t *catom = PDDL_COND_CAST(cc, atom);
                if (catom->pred == a->pred){
                    pddlCondArrAdd(atoms, c);
                    break;
                }
            }
        }
    }
}


static void renameVars(int from, int to,
                       const pddl_lifted_mgroup_t *cand,
                       int *cand_arg,
                       const pddl_action_t *action,
                       int *action_arg)
{
    for (int i = 0; i < cand->param.param_size; ++i){
        if (cand_arg[i] == from)
            cand_arg[i] = to;
    }
    for (int i = 0; i < action->param.param_size; ++i){
        if (action_arg[i] == from)
            action_arg[i] = to;
    }
}

static int setVarNames(const pddl_lifted_mgroup_t *cand,
                       const pddl_cond_atom_t *c1,
                       const pddl_action_t *action,
                       const pddl_cond_atom_t *a1,
                       int obj_offset,
                       int *next_name,
                       int *cand_var,
                       int *action_var)
{
    for (int i = 0; i < c1->arg_size; ++i){
        int cparam = c1->arg[i].param;
        if (a1->arg[i].param >= 0){
            int aparam = a1->arg[i].param;
            if (cand_var[cparam] == 0 && action_var[aparam] == 0){
                cand_var[cparam] = action_var[aparam] = *next_name;
                *next_name += 1;

            }else if (cand_var[cparam] == 0 && action_var[aparam] > 0){
                cand_var[cparam] = action_var[aparam];

            }else if (cand_var[cparam] > 0 && action_var[aparam] == 0){
                action_var[aparam] = cand_var[cparam];

            }else if (cand_var[cparam] != action_var[aparam]){
                //  cand_var[cparam] > 0 && action_var[aparam] > 0
                if (cand_var[cparam] >= obj_offset
                        && action_var[aparam] >= obj_offset){
                    return 0;

                }else if (cand_var[cparam] >= obj_offset){
                    renameVars(action_var[aparam], cand_var[cparam],
                               cand, cand_var, action, action_var);

                }else{
                    renameVars(cand_var[cparam], action_var[aparam],
                               cand, cand_var, action, action_var);
                }
            }
        }else{
            int obj_id = obj_offset + a1->arg[i].obj;
            if (cand_var[cparam] == 0){
                cand_var[cparam] = obj_id;
            }else if (cand_var[cparam] >= obj_offset){
                if (cand_var[cparam] != obj_id)
                    return 0;
            }else{
                renameVars(cand_var[cparam], obj_id,
                           cand, cand_var, action, action_var);
            }
        }
    }

    return 1;
}

static int argTypesAreValid(const pddl_types_t *ts,
                            const pddl_action_t *action,
                            const int *action_arg)
{
    for (int i = 0; i < action->param.param_size; ++i){
        if (action_arg[i] == 0)
            continue;
        int type1 = action->param.param[i].type;

        for (int j = i + 1; j < action->param.param_size; ++j){
            int type2 = action->param.param[j].type;
            if (action_arg[i] == action_arg[j]
                    && pddlTypesAreDisjunct(ts, type1, type2))
                return 0;
        }
    }

    return 1;
}

struct inequality_pre_hold {
    int pred_eq;
    int obj_offset;
    const int *action_arg;
    int ret;
};

static int _inequalityPreHold(pddl_cond_t *c, void *ud)
{
    struct inequality_pre_hold *ctx = ud;
    if (c->type == PDDL_COND_ATOM){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(c, atom);
        if (a->pred == ctx->pred_eq && a->neg){
            if (a->arg[0].param >= 0 && a->arg[1].param >= 0){
                int p0 = a->arg[0].param;
                int p1 = a->arg[1].param;
                if (ctx->action_arg[p0] > 0
                        && ctx->action_arg[p1] > 0
                        && ctx->action_arg[p0] == ctx->action_arg[p1]){
                    ctx->ret = -1;
                    return -2;
                }
            }else if (a->arg[0].param >= 0){
                int p0 = a->arg[0].param;
                int o1 = ctx->obj_offset + a->arg[1].obj;
                if (ctx->action_arg[p0] > 0 && ctx->action_arg[p0] == o1){
                    ctx->ret = -1;
                    return -2;
                }

            }else if (a->arg[1].param >= 0){
                int o0 = ctx->obj_offset + a->arg[0].obj;
                int p1 = a->arg[1].param;
                if (ctx->action_arg[p1] > 0 && ctx->action_arg[p1] == o0){
                    ctx->ret = -1;
                    return -2;
                }
            }
        }
    }

    return 0;
}

static int inequalityPreHold(const pddl_action_t *action,
                             int pred_eq,
                             int obj_offset,
                             const int *action_arg)
{
    struct inequality_pre_hold ctx = {
        pred_eq,
        obj_offset,
        action_arg,
        0
    };

    if (pred_eq < 0)
        return 1;

    pddlCondTraverse(action->pre, _inequalityPreHold, NULL, &ctx);
    if (ctx.ret < 0)
        return 0;
    return 1;
}

static int canUnifyActionAddEffectPairCand(const pddl_t *pddl,
                                           const pddl_lifted_mgroup_t *cand,
                                           const pddl_cond_atom_t *c1,
                                           const pddl_cond_atom_t *c2,
                                           const pddl_action_t *action,
                                           const pddl_cond_atom_t *a1,
                                           const pddl_cond_atom_t *a2)
{
    int obj_offset = 1 + cand->param.param_size + action->param.param_size;
    int next_name = 1;
    int cand_var[cand->param.param_size];
    int action_var[action->param.param_size];

    bzero(cand_var, sizeof(int) * cand->param.param_size);
    bzero(action_var, sizeof(int) * action->param.param_size);

    if (!setVarNames(cand, c1, action, a1, obj_offset,
                     &next_name, cand_var, action_var)){
        return 0;
    }

    // Empty counted variables because they can be bound to something else
    // now
    for (int i = 0; i < cand->param.param_size; ++i){
        if (cand->param.param[i].is_counted_var)
            cand_var[i] = 0;
    }

    if (!setVarNames(cand, c2, action, a2, obj_offset,
                     &next_name, cand_var, action_var)){
        return 0;
    }

    // If two variables has the same name, but the corresponding types are
    // disjunct, then we cannot unify the atoms
    if (!argTypesAreValid(&pddl->type, action, action_var))
        return 0;

    // Check inequality predicates: we cannot assign the same name to two
    // arguments that cannot be same
    if (!inequalityPreHold(action, pddl->pred.eq_pred, obj_offset, action_var))
        return 0;

    if (a1->pred == a2->pred){
        for (int i = 0; i < a1->arg_size; ++i){
            int a1param = a1->arg[i].param;
            int a1obj = a1->arg[i].obj;
            int a2param = a2->arg[i].param;
            int a2obj = a2->arg[i].obj;
            if (a1param >= 0 && a2param >= 0){
                if (action_var[a1param] != action_var[a2param])
                    return 1;
            }else if (a1param >= 0){
                if (action_var[a1param] != obj_offset + a2obj)
                    return 1;
            }else if (a2param >= 0){
                if (action_var[a2param] != obj_offset + a1obj)
                    return 1;
            }else{
                if (a1obj != a2obj)
                    return 1;
            }
        }

        return 0;
    }

    return 1;
}

static int canUnifyActionAddEffectPair(const pddl_t *pddl,
                                       const pddl_lifted_mgroup_t *cand,
                                       const pddl_action_t *action,
                                       const pddl_cond_atom_t *a1,
                                       const pddl_cond_atom_t *a2)
{
    for (int c1i = 0; c1i < cand->cond.size; ++c1i){
        const pddl_cond_t *c1 = cand->cond.cond[c1i];
        const pddl_cond_atom_t *cand1 = PDDL_COND_CAST(c1, atom);
        if (cand1->pred != a1->pred)
            continue;

        for (int c2i = 0; c2i < cand->cond.size; ++c2i){
            const pddl_cond_t *c2 = cand->cond.cond[c2i];
            const pddl_cond_atom_t *cand2 = PDDL_COND_CAST(c2, atom);
            if (cand2->pred != a2->pred)
                continue;
            if (canUnifyActionAddEffectPairCand(pddl, cand, cand1, cand2,
                                                action, a1, a2)){
                return 1;
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

        int cont = 0;
        for (int i = 0; i < cand1->arg_size; ++i){
            ASSERT(cand1->arg[i].obj == PDDL_OBJ_ID_UNDEF);
            ASSERT(a1->arg[i].obj >= 0);
            int param = cand1->arg[i].param;
            if (!cand->param.param[param].is_counted_var){
                if (arg[param] == PDDL_OBJ_ID_UNDEF){
                    arg[param] = a1->arg[i].obj;
                }else if (arg[param] != a1->arg[i].obj){
                    cont = 1;
                    break;
                }
            }
        }

        if (cont)
            continue;

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

int pddlLiftedMGroupIsInitTooHeavy(const pddl_lifted_mgroup_t *cand,
                                   const pddl_t *pddl)
{
    pddl_cond_arr_t atom_arr = PDDL_COND_ARR_INIT;

    collectCandAtomsFromAnd(cand, pddl->init, &atom_arr, 1, 0);

    for (int i = 0; i < atom_arr.size; ++i){
        const pddl_cond_t *c1 = atom_arr.cond[i];
        const pddl_cond_atom_t *a1 = PDDL_COND_CAST(c1, atom);
        for (int j = i + 1; j < atom_arr.size; ++j){
            const pddl_cond_t *c2 = atom_arr.cond[j];
            const pddl_cond_atom_t *a2 = PDDL_COND_CAST(c2, atom);
            if (canUnifyToGroundedPair(cand, a1, a2)){
                pddlCondArrFree(&atom_arr);
                return 1;
            }
        }
    }
    pddlCondArrFree(&atom_arr);

    return 0;
}


int pddlLiftedMGroupIsActionTooHeavy(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id)
{
    const pddl_action_t *action = pddl->action.action + action_id;
    pddl_cond_arr_t add_eff = PDDL_COND_ARR_INIT;

    if (action->eff == NULL || action->eff->type != PDDL_COND_AND)
        return 0;

    collectCandAtomsFromAnd(cand, PDDL_COND_CAST(action->eff, part),
                            &add_eff, 1, 0);
    if (add_eff.size <= 1){
        pddlCondArrFree(&add_eff);
        return 0;
    }

    for (int i = 0; i < add_eff.size; ++i){
        const pddl_cond_t *c1 = add_eff.cond[i];
        const pddl_cond_atom_t *a1 = PDDL_COND_CAST(c1, atom);
        for (int j = i + 1; j < add_eff.size; ++j){
            const pddl_cond_t *c2 = add_eff.cond[j];
            const pddl_cond_atom_t *a2 = PDDL_COND_CAST(c2, atom);
            if (canUnifyActionAddEffectPair(pddl, cand, action, a1, a2)){
                pddlCondArrFree(&add_eff);
                return 1;
            }
        }
    }

    pddlCondArrFree(&add_eff);
    return 0;
}

struct is_balanced_ctx {
    const pddl_lifted_mgroup_t *cand;
    pddl_cond_arr_t *pre;
    pddl_cond_arr_t *del_eff;
    pddl_cond_arr_t *add_eff;
};

int pddlLiftedMGroupIsBalanced(const pddl_lifted_mgroup_t *cand,
                               const pddl_action_t *action)
{
    pddl_cond_arr_t pre, del_eff, add_eff;

    pddlCondArrInit(&pre);
    pddlCondArrInit(&del_eff);
    pddlCondArrInit(&add_eff);

    pddlCondArrFree(&pre);
    pddlCondArrFree(&del_eff);
    pddlCondArrFree(&add_eff);
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
