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

#include "boruvka/sort.h"
#include "pddl/pddl.h"
#include "pddl/lifted_mgroup.h"
#include "assert.h"

/** Returns true if the candidate contains atom of the specified predicate */
static int candHasPred(const pddl_lifted_mgroup_t *cand, int pred)
{
    for (int i = 0; i < cand->cond.size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(cand->cond.cond[i], atom);
        if (a->pred == pred)
            return 1;
    }
    return 0;
}

static void collectCandAtomsFromAnd(const pddl_lifted_mgroup_t *cand,
                                    const pddl_cond_t *cond,
                                    pddl_cond_arr_t *atoms,
                                    int positive,
                                    int negative)
{
    if (cond == NULL)
        return;

    if (cond->type == PDDL_COND_AND){
        bor_list_t *item;
        const pddl_cond_part_t *and = PDDL_COND_CAST(cond, part);
        BOR_LIST_FOR_EACH(&and->part, item){
            const pddl_cond_t *c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            if (c->type == PDDL_COND_ATOM){
                const pddl_cond_atom_t *a = PDDL_COND_CAST(c, atom);
                if ((a->neg && !negative) || (!a->neg && !positive))
                    continue;

                if (candHasPred(cand, a->pred))
                    pddlCondArrAdd(atoms, &a->cls);
            }
        }

    }else if (cond->type == PDDL_COND_ATOM){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(cond, atom);
        if (candHasPred(cand, a->pred))
            pddlCondArrAdd(atoms, &a->cls);
    }
}

static void collectNonCandDelEffs(const pddl_lifted_mgroup_t *cand,
                                  const pddl_cond_t *eff,
                                  pddl_cond_arr_t *atoms)
{
    if (eff == NULL)
        return;

    if (eff->type == PDDL_COND_AND){
        bor_list_t *item;
        const pddl_cond_part_t *and = PDDL_COND_CAST(eff, part);
        BOR_LIST_FOR_EACH(&and->part, item){
            const pddl_cond_t *c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            if (c->type == PDDL_COND_ATOM){
                const pddl_cond_atom_t *a = PDDL_COND_CAST(c, atom);
                if (a->neg && !candHasPred(cand, a->pred))
                    pddlCondArrAdd(atoms, &a->cls);
            }
        }

    }else if (eff->type == PDDL_COND_ATOM){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(eff, atom);
        if (a->neg && !candHasPred(cand, a->pred))
            pddlCondArrAdd(atoms, &a->cls);
    }
}

static int checkExactMatchAtom(const pddl_action_t *action,
                               const int *action_var,
                               int obj_offset,
                               const pddl_cond_atom_t *atom1,
                               const pddl_cond_atom_t *atom2)
{
    if (atom1->pred != atom2->pred)
        return 0;

    for (int ai = 0; ai < atom1->arg_size; ++ai){
        int atom1_param = atom1->arg[ai].param;
        int atom1_obj = obj_offset + atom1->arg[ai].obj;
        int atom2_param = atom2->arg[ai].param;
        int atom2_obj = obj_offset + atom2->arg[ai].obj;
        if (atom1_param >= 0){
            if (atom2_param >= 0){
                if (action_var[atom1_param] != action_var[atom2_param])
                    return 0;
            }else{
                if (action_var[atom1_param] != atom2_obj)
                    return 0;
            }
        }else{
            if (atom2_param >= 0){
                if (action_var[atom2_param] != atom1_obj)
                    return 0;
            }else{
                if (atom1_obj != atom2_obj)
                    return 0;
            }
        }
    }

    return 1;
}

static int checkExactMatch(const pddl_action_t *action,
                           const int *action_var,
                           int obj_offset,
                           const pddl_cond_t *cond,
                           const pddl_cond_atom_t *atom)
{
    if (cond->type == PDDL_COND_ATOM){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(cond, atom);
        return checkExactMatchAtom(action, action_var, obj_offset, a, atom);

    }else if (cond->type == PDDL_COND_AND){
        bor_list_t *item;
        const pddl_cond_part_t *and = PDDL_COND_CAST(cond, part);
        BOR_LIST_FOR_EACH(&and->part, item){
            const pddl_cond_t *c = BOR_LIST_ENTRY(item, pddl_cond_t, conn);
            if (c->type == PDDL_COND_ATOM){
                const pddl_cond_atom_t *a = PDDL_COND_CAST(c, atom);
                if (checkExactMatchAtom(action, action_var, obj_offset,
                                        a, atom))
                    return 1;
            }
        }

        return 0;
    }

    return 0;
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

void pddlLiftedMGroupInitCopy(pddl_lifted_mgroup_t *dst,
                              const pddl_lifted_mgroup_t *src)
{
    bzero(dst, sizeof(*dst));
    pddlParamsInitCopy(&dst->param, &src->param);
    for (int i = 0; i < src->cond.size; ++i)
        pddlCondArrAdd(&dst->cond, pddlCondClone(src->cond.cond[i]));
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

    collectCandAtomsFromAnd(cand, &pddl->init->cls, &atom_arr, 1, 0);

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

    collectCandAtomsFromAnd(cand, action->eff, &add_eff, 1, 0);
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

static int isBalancedWith(const pddl_lifted_mgroup_t *cand,
                          const pddl_cond_atom_t *catom,
                          const pddl_action_t *action,
                          const pddl_cond_atom_t *del_eff,
                          int obj_offset,
                          int next_name,
                          const int *in_cand_var,
                          const int *in_action_var)
{
    int cand_var[cand->param.param_size];
    int action_var[action->param.param_size];

    memcpy(cand_var, in_cand_var, sizeof(int) * cand->param.param_size);
    memcpy(action_var, in_action_var, sizeof(int) * action->param.param_size);

    // Empty counted variables
    for (int i = 0; i < cand->param.param_size; ++i){
        if (cand->param.param[i].is_counted_var)
            cand_var[i] = 0;
    }

    for (int ai = 0; ai < catom->arg_size; ++ai){
        int cparam = catom->arg[ai].param;
        int dparam = del_eff->arg[ai].param;
        int dobj = obj_offset + del_eff->arg[ai].obj;
        if (cand->param.param[cparam].is_counted_var){
            // Counted variables can be instantiated with anything...
            if (dparam >= 0){
                if (action_var[dparam] == 0)
                    action_var[dparam] = next_name++;
                cand_var[cparam] = action_var[dparam];
            }else{
                cand_var[cparam] = dobj;
            }
        }else{
            // If it is not counted variable then
            // 1. either it has assigned a value that del_eff must respect and
            //    if not possible, this delete effect cannot balance the
            //    add effect
            // 2. or it doesn't have assigned any value, in which case we can
            //    reject this delete effect straight away, because
            //    either a) the delete effect already has assigned a value
            //    therefore we can instantiate the candidate with a
            //    different value so that the delete effect does not agree,
            //    or b) or the delete effect doesn't have assigned any
            //    value, therefore we can, again, instantiate the action
            //    differently from the candidate
            if (dparam >= 0){
                if (action_var[dparam] > 0){
                    if (cand_var[cparam] != action_var[dparam]){
                        // Accounts for 1. and 2a.
                        return 0;
                    }

                }else{ // action_var[dparam] == 0
                    // Accounts for 2b.
                    return 0;
                }

            }else{
                if (cand_var[cparam] != dobj){
                    // Accounts for 1. and 2a.
                    return 0;
                }
            }
        }
    }

    // Now we have assigned names to action variables and we must check
    // that there is a precondition exactly matching the delete effect so
    // we can be sure that the delete effect is present in the state the
    // action is applied on, i.e., that the delete effect really balances
    // the add effect.
    if (checkExactMatch(action, action_var, obj_offset, action->pre, del_eff))
        return 1;

    // If we did not find a matching precondition, we report that the
    // delete effect cannot balance the add effect.
    return 0;
}

static void addRefinedCandidate(const pddl_lifted_mgroup_t *cand_in,
                                const pddl_cond_atom_t *atom,
                                const int *atom_params,
                                pddl_lifted_mgroups_t *refine)
{
    pddl_lifted_mgroup_t new_cand;
    pddl_cond_atom_t *new_atom;

    // Create a copy of the candidate
    pddlLiftedMGroupInitCopy(&new_cand, cand_in);

    // Construct a new atom that will be added to the new candidate
    new_atom = pddlCondNewEmptyAtom(atom->arg_size);
    new_atom->pred = atom->pred;
    for (int i = 0; i < atom->arg_size; ++i){
        if (atom_params[i] < 0){
            new_atom->arg[i].param = new_cand.param.param_size;
            pddl_param_t *param = pddlParamsAdd(&new_cand.param);
            param->is_counted_var = 1;
        }else{
            new_atom->arg[i].param = atom_params[i];
        }
    }
    pddlCondArrAdd(&new_cand.cond, &new_atom->cls);

    pddlLiftedMGroupsAdd(refine, &new_cand);
    pddlLiftedMGroupFree(&new_cand);
}

static void refineCandidateWithDelEff(const pddl_lifted_mgroup_t *cand,
                                      const pddl_action_t *action,
                                      int obj_offset,
                                      const int *cand_var,
                                      const int *action_var_in,
                                      const pddl_cond_atom_t *del_eff,
                                      int *del_eff_params,
                                      int del_eff_argi,
                                      int num_counted_vars,
                                      int next_name,
                                      pddl_lifted_mgroups_t *refined)
{
    if (del_eff_argi == del_eff->arg_size){
        if (checkExactMatch(action, action_var_in, obj_offset,
                            action->pre, del_eff)){
            addRefinedCandidate(cand, del_eff, del_eff_params, refined);
        }

    }else if (del_eff->arg[del_eff_argi].obj != PDDL_OBJ_ID_UNDEF
                || action_var_in[del_eff->arg[del_eff_argi].param] >= obj_offset
                || action_var_in[del_eff->arg[del_eff_argi].param] == 0){
        // This must be counted variable
        del_eff_params[del_eff_argi] = -1;
        if (num_counted_vars >= 1)
            return;

        int action_var[action->param.param_size];
        memcpy(action_var, action_var_in,
               sizeof(int) * action->param.param_size);
        if (action_var_in[del_eff->arg[del_eff_argi].param] == 0)
            action_var[del_eff->arg[del_eff_argi].param] = next_name++;

        refineCandidateWithDelEff(cand, action, obj_offset,
                                  cand_var, action_var,
                                  del_eff, del_eff_params, del_eff_argi + 1,
                                  num_counted_vars + 1, next_name,
                                  refined);

    }else{
        int tried_counted_var = 0;
        int avar = action_var_in[del_eff->arg[del_eff_argi].param];
        for (int ci = 0; ci < cand->param.param_size; ++ci){
            if (cand_var[ci] != avar)
                continue;
            if (cand->param.param[ci].is_counted_var){
                if (num_counted_vars == 0){
                    tried_counted_var = 1;
                    del_eff_params[del_eff_argi] = -1;
                    refineCandidateWithDelEff(cand, action, obj_offset,
                                              cand_var, action_var_in,
                                              del_eff, del_eff_params,
                                              del_eff_argi + 1,
                                              num_counted_vars + 1, next_name,
                                              refined);
                }
            }else{
                del_eff_params[del_eff_argi] = ci;
                refineCandidateWithDelEff(cand, action, obj_offset,
                                          cand_var, action_var_in,
                                          del_eff, del_eff_params,
                                          del_eff_argi + 1,
                                          num_counted_vars, next_name,
                                          refined);
            }
        }

        if (num_counted_vars == 0 && !tried_counted_var){
            del_eff_params[del_eff_argi] = -1;
            refineCandidateWithDelEff(cand, action, obj_offset,
                                      cand_var, action_var_in,
                                      del_eff, del_eff_params,
                                      del_eff_argi + 1,
                                      num_counted_vars + 1, next_name,
                                      refined);
        }
    }
}

static void refineCandidate(const pddl_lifted_mgroup_t *cand,
                            const pddl_action_t *action,
                            int obj_offset,
                            const int *cand_var,
                            const int *action_var,
                            int next_name,
                            pddl_lifted_mgroups_t *refined)
{
    pddl_cond_arr_t del_effs = PDDL_COND_ARR_INIT;

    collectNonCandDelEffs(cand, action->eff, &del_effs);

    for (int ai = 0; ai < del_effs.size; ++ai){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(del_effs.cond[ai], atom);
        int del_eff_params[a->arg_size];
        refineCandidateWithDelEff(cand, action, obj_offset,
                                  cand_var, action_var,
                                  a, del_eff_params, 0, 0, next_name,
                                  refined);
    }

    pddlCondArrFree(&del_effs);
}

static int isAddEffBalanced(const pddl_lifted_mgroup_t *cand,
                            const pddl_t *pddl,
                            const pddl_action_t *action,
                            const pddl_cond_atom_t *add_eff,
                            const pddl_cond_arr_t *del_effs,
                            const pddl_cond_arr_t *pres,
                            pddl_lifted_mgroups_t *refined)
{
    int obj_offset = 1 + cand->param.param_size + action->param.param_size;
    int next_name = 1;
    int cand_var[cand->param.param_size];
    int action_var[action->param.param_size];

    for (int ci = 0; ci < cand->cond.size; ++ci){
        const pddl_cond_t *cc = cand->cond.cond[ci];
        const pddl_cond_atom_t *cand_atom = PDDL_COND_CAST(cc, atom);
        if (cand_atom->pred != add_eff->pred)
            continue;

        bzero(cand_var, sizeof(int) * cand->param.param_size);
        bzero(action_var, sizeof(int) * action->param.param_size);
        if (!setVarNames(cand, cand_atom, action, add_eff, obj_offset,
                         &next_name, cand_var, action_var)
                || !argTypesAreValid(&pddl->type, action, action_var)
                || !inequalityPreHold(action, pddl->pred.eq_pred,
                                      obj_offset, action_var)){
            continue;
        }

        // We managed to find one covered add effect -- fast check that
        // there is something with which we can balance it
        if (del_effs->size == 0 || pres->size == 0){
            if (refined != NULL){
                refineCandidate(cand, action, obj_offset,
                                cand_var, action_var,
                                next_name, refined);
            }
            return 0;
        }

        int is_balanced = 0;
        for (int di = 0; !is_balanced && di < del_effs->size; ++di){
            const pddl_cond_t *c = del_effs->cond[di];
            const pddl_cond_atom_t *del_eff = PDDL_COND_CAST(c, atom);
            for (int j = 0; !is_balanced && j < cand->cond.size; ++j){
                const pddl_cond_t *cc = cand->cond.cond[j];
                const pddl_cond_atom_t *balance_cand = PDDL_COND_CAST(cc, atom);
                if (balance_cand->pred != del_eff->pred)
                    continue;

                if (isBalancedWith(cand, balance_cand, action, del_eff,
                                   obj_offset, next_name,
                                   cand_var, action_var)){
                    is_balanced = 1;
                }
            }
        }

        if (!is_balanced){
            if (refined != NULL){
                refineCandidate(cand, action, obj_offset,
                                cand_var, action_var,
                                next_name, refined);
            }
            return 0;
        }
    }

    return 1;
}

int pddlLiftedMGroupIsActionBalanced(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id,
                                     pddl_lifted_mgroups_t *refined)
{
    const pddl_action_t *action = pddl->action.action + action_id;
    pddl_cond_arr_t pre = PDDL_COND_ARR_INIT;
    pddl_cond_arr_t del_eff = PDDL_COND_ARR_INIT;
    pddl_cond_arr_t add_eff = PDDL_COND_ARR_INIT;

    collectCandAtomsFromAnd(cand, action->eff, &add_eff, 1, 0);
    if (add_eff.size == 0){
        pddlCondArrFree(&add_eff);
        return 1;
    }

    collectCandAtomsFromAnd(cand, action->eff, &del_eff, 0, 1);
    collectCandAtomsFromAnd(cand, action->pre, &pre, 1, 0);

    for (int i = 0; i < add_eff.size; ++i){
        const pddl_cond_atom_t *add = PDDL_COND_CAST(add_eff.cond[i], atom);
        if (!isAddEffBalanced(cand, pddl, action, add, &del_eff, &pre,
                              refined)){
            pddlCondArrFree(&pre);
            pddlCondArrFree(&del_eff);
            pddlCondArrFree(&add_eff);
            return 0;
        }
    }

    pddlCondArrFree(&pre);
    pddlCondArrFree(&del_eff);
    pddlCondArrFree(&add_eff);
    return 1;
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
                    fprintf(fout, " C%d", param_id);
                }else{
                    fprintf(fout, " V%d", param_id);
                }
            }else{
                fprintf(fout, " %s", pddl->obj.obj[atom->arg[j].obj].name);
            }
        }
    }

    fprintf(fout, "}\n");
}


void pddlLiftedMGroupsInit(pddl_lifted_mgroups_t *lm)
{
    bzero(lm, sizeof(*lm));
}

void pddlLiftedMGroupsFree(pddl_lifted_mgroups_t *lm)
{
    for (int i = 0; i < lm->mgroup_size; ++i)
        pddlLiftedMGroupFree(lm->mgroup + i);
    if (lm->mgroup != NULL)
        BOR_FREE(lm->mgroup);
}

static int cmpAtoms(const void *a, const void *b, void *_)
{
    const pddl_cond_t *c1 = *(const pddl_cond_t **)a;
    const pddl_cond_t *c2 = *(const pddl_cond_t **)b;
    const pddl_cond_atom_t *a1 = PDDL_COND_CAST(c1, atom);
    const pddl_cond_atom_t *a2 = PDDL_COND_CAST(c2, atom);
    return a1->pred - a2->pred;
}

void pddlLiftedMGroupsAdd(pddl_lifted_mgroups_t *lm,
                          const pddl_lifted_mgroup_t *lmg)
{
    if (lm->mgroup_size == lm->mgroup_alloc){
        if (lm->mgroup_alloc == 0)
            lm->mgroup_alloc = 2;
        lm->mgroup_alloc *= 2;
        lm->mgroup = BOR_REALLOC_ARR(lm->mgroup, pddl_lifted_mgroup_t,
                                     lm->mgroup_alloc);
    }

    pddl_lifted_mgroup_t *add = lm->mgroup + lm->mgroup_size++;
    pddlLiftedMGroupInitCopy(add, lmg);
    borSort(add->cond.cond, add->cond.size, sizeof(const pddl_cond_t *),
            cmpAtoms, NULL);
}

static int cmpLiftedMGroups(const void *a, const void *b, void *_)
{
    const pddl_lifted_mgroup_t *m1 = a;
    const pddl_lifted_mgroup_t *m2 = b;
    int cmp = m1->cond.size - m2->cond.size;
    for (int i = 0; cmp == 0 && i < m1->cond.size; ++i){
        const pddl_cond_t *c1 = m1->cond.cond[i];
        const pddl_cond_atom_t *a1 = PDDL_COND_CAST(c1, atom);
        const pddl_cond_t *c2 = m2->cond.cond[i];
        const pddl_cond_atom_t *a2 = PDDL_COND_CAST(c2, atom);
        cmp = a1->pred - a2->pred;
        for (int j = 0; cmp == 0 && j < a1->arg_size; ++j){
            cmp = a1->arg[j].param - a2->arg[j].param;
            if (cmp == 0)
                cmp = a1->arg[j].obj - a2->arg[j].obj;
        }
    }
    if (cmp == 0)
        cmp = m1->param.param_size - m2->param.param_size;
    for (int i = 0; cmp == 0 && i < m1->param.param_size; ++i){
        cmp = m1->param.param[i].is_counted_var
                - m2->param.param[i].is_counted_var;
    }

    return cmp;
}

static int mgroupsEq(const pddl_lifted_mgroup_t *m1,
                     const pddl_lifted_mgroup_t *m2)
{
    return cmpLiftedMGroups(m1, m2, NULL) == 0;
}

void pddlLiftedMGroupsSortAndUniq(pddl_lifted_mgroups_t *lm)
{
    if (lm->mgroup_size == 0)
        return;

    borSort(lm->mgroup, lm->mgroup_size, sizeof(pddl_lifted_mgroup_t),
            cmpLiftedMGroups, NULL);

    int ins = 1;
    for (int i = 1; i < lm->mgroup_size; ++i){
        if (mgroupsEq(lm->mgroup + ins - 1, lm->mgroup + i)){
            pddlLiftedMGroupFree(lm->mgroup + i);
        }else{
            lm->mgroup[ins++] = lm->mgroup[i];
        }
    }
    lm->mgroup_size = ins;
}

void pddlLiftedMGroupsPrint(const pddl_t *pddl,
                            const pddl_lifted_mgroups_t *lm,
                            FILE *fout)
{
    fprintf(fout, "< ");
    for (int i = 0; i < lm->mgroup_size; ++i){
        if (i > 0)
            fprintf(fout, ", ");
        pddlLiftedMGroupPrint(pddl, lm->mgroup + i, fout);
    }
    fprintf(fout, ">\n");
}
