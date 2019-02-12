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

struct ce_atom {
    const pddl_cond_t *pre;
    const pddl_cond_atom_t *atom;
};
typedef struct ce_atom ce_atom_t;

#define CE_ATOM(NAME, PRE, ATOM) \
    ce_atom_t NAME = { (PRE), (ATOM) }

struct ctx {
    const pddl_t *pddl;
    const pddl_lifted_mgroup_t *cand;
    const pddl_action_t *action;
    int obj_offset;
    int *action_var;
    int *cand_var;
    int next_name;
};
typedef struct ctx ctx_action_t;

#define CTX_ACTION(NAME, PDDL, ACTION_ID, CAND) \
    int __action_var[(PDDL)->action.action[(ACTION_ID)].param.param_size]; \
    int __cand_var[(CAND)->param.param_size]; \
    ctx_action_t NAME = { \
        (PDDL), \
        (CAND), \
        (PDDL)->action.action + (ACTION_ID), \
        1 + (CAND)->param.param_size \
            + (PDDL)->action.action[(ACTION_ID)].param.param_size, \
        __action_var, \
        __cand_var, \
        1 \
    }

#define CTX_RESET(CTX) \
    do { \
        (CTX)->next_name = 1; \
        bzero((CTX)->cand_var, sizeof(int) * (CTX)->cand->param.param_size); \
        bzero((CTX)->action_var, \
                sizeof(int) * (CTX)->action->param.param_size); \
    } while (0)

#define CTX_PUSH(CTX, NAME) \
    int __action_var##NAME[(CTX)->action->param.param_size]; \
    int __cand_var##NAME[(CTX)->cand->param.param_size]; \
    ctx_action_t NAME = { \
        (CTX)->pddl, \
        (CTX)->cand, \
        (CTX)->action, \
        (CTX)->obj_offset, \
        __action_var##NAME, \
        __cand_var##NAME, \
        (CTX)->next_name \
    }; \
    memcpy(NAME.cand_var, (CTX)->cand_var, \
            sizeof(int) * (CTX)->cand->param.param_size); \
    memcpy(NAME.action_var, (CTX)->action_var, \
            sizeof(int) * (CTX)->action->param.param_size)

#define FOR_EACH_CAND(CAND, C) \
    for (int ___i = 0; \
            ___i < (CAND)->cond.size \
                && ((C) = PDDL_COND_CAST((CAND)->cond.cond[___i], atom)); \
                ++___i)


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

/** Returns true if atom_i's atom from the candidate has only counted
 *  variables */
static int candAtomHasOnlyCountedVars(const pddl_lifted_mgroup_t *cand,
                                      int atom_i)
{
    const pddl_cond_atom_t *a = PDDL_COND_CAST(cand->cond.cond[atom_i], atom);
    for (int i = 0; i < a->arg_size; ++i){
        if (a->arg[i].obj != PDDL_OBJ_ID_UNDEF
                || !cand->param.param[a->arg[i].param].is_counted_var)
            return 0;
    }
    return 1;
}

static void selectOnlyAtomsWithCountedVars(pddl_lifted_mgroup_t *dst,
                                           const pddl_lifted_mgroup_t *src)
{
    pddlLiftedMGroupInitCopy(dst, src);
    for (int i = 0; i < dst->cond.size; ++i){
        if (!candAtomHasOnlyCountedVars(dst, i)){
            pddlCondDel((pddl_cond_t *)dst->cond.cond[i]);
            dst->cond.cond[i] = NULL;
        }
    }
    int ins = 0;
    for (int i = 0; i < dst->cond.size; ++i){
        if (dst->cond.cond[i] != NULL)
            dst->cond.cond[ins++] = dst->cond.cond[i];
    }
    dst->cond.size = ins;
}

static int isInitTooHeavyForCountedVars(const pddl_lifted_mgroup_t *cand_in,
                                        const pddl_t *pddl)
{
    int ret = 0;

    pddl_lifted_mgroup_t cand;
    selectOnlyAtomsWithCountedVars(&cand, cand_in);

    if (cand.cond.size > 0)
        ret = pddlLiftedMGroupIsInitTooHeavy(&cand, pddl);

    pddlLiftedMGroupFree(&cand);
    return ret;
}


/** Returns value corresponding to the specified argument */
static int atomArgValue(const ctx_action_t *ctx,
                        const pddl_cond_atom_t *atom,
                        int argi)
{
    int param = atom->arg[argi].param;
    if (param >= 0)
        return ctx->action_var[param];
    return atom->arg[argi].obj + ctx->obj_offset;
}

/** Returns true if the atoms are equal under the given variable assignment */
static int atomsEq(const ctx_action_t *ctx,
                   const pddl_cond_atom_t *atom1,
                   const pddl_cond_atom_t *atom2)
{
    if (atom1->pred != atom2->pred)
        return 0;

    for (int ai = 0; ai < atom1->arg_size; ++ai){
        if (atomArgValue(ctx, atom1, ai) != atomArgValue(ctx, atom2, ai))
            return 0;
    }
    return 1;
}

/** Returns true if the exactly same atom can be found in a->pre */
static int atomInPre(const ctx_action_t *ctx,
                     const pddl_cond_t *pre,
                     const pddl_cond_atom_t *atom)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a2;

    if (pre == NULL)
        return 0;

    PDDL_COND_FOR_EACH_ATOM(pre, &it, a2){
        if (!a2->neg && atomsEq(ctx, a2, atom))
            return 1;
    }
    return 0;
}
static int atomInArrGroundedWithArgs(const pddl_cond_atom_t *atom,
                                     const pddl_cond_arr_t *arr,
                                     const pddl_obj_id_t *args)
{
    for (int i = 0; i < arr->size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(arr->cond[i], atom);
        if (a->pred != atom->pred)
            continue;
        int match = 1;
        for (int ai = 0; ai < atom->arg_size; ++ai){
            pddl_obj_id_t atom_obj = atom->arg[ai].obj;
            if (atom_obj < 0)
                atom_obj = args[atom->arg[ai].param];
            pddl_obj_id_t a_obj = a->arg[ai].obj;
            if (a_obj < 0)
                a_obj = args[a->arg[ai].param];

            if (atom_obj < 0 || a_obj < 0 || atom_obj != a_obj){
                match = 0;
                break;
            }
        }
        if (match)
            return 1;
    }
    return 0;
}


static void renameVars(ctx_action_t *ctx, int from, int to)
{
    for (int i = 0; i < ctx->cand->param.param_size; ++i){
        if (ctx->cand_var[i] == from)
            ctx->cand_var[i] = to;
    }
    for (int i = 0; i < ctx->action->param.param_size; ++i){
        if (ctx->action_var[i] == from)
            ctx->action_var[i] = to;
    }
}

static int unifyAtoms(ctx_action_t *ctx,
                      const pddl_cond_atom_t *c1,
                      const pddl_cond_atom_t *a1)
{
    // Check that types are compatible.
    for (int i = 0; i < c1->arg_size; ++i){
        int cparam = c1->arg[i].param;
        ASSERT_RUNTIME(cparam >= 0);
        int ctype = ctx->cand->param.param[cparam].type;
        if (a1->arg[i].param >= 0){
            int aparam = a1->arg[i].param;
            int atype = ctx->action->param.param[aparam].type;
            if (pddlTypesAreDisjunct(&ctx->pddl->type, ctype, atype))
                return 0;
        }else{
            if (!pddlTypesObjHasType(&ctx->pddl->type, ctype, a1->arg[i].obj))
                return 0;
        }
    }

    for (int i = 0; i < c1->arg_size; ++i){
        int cparam = c1->arg[i].param;
        if (a1->arg[i].param >= 0){
            int aparam = a1->arg[i].param;
            if (ctx->cand_var[cparam] == 0 && ctx->action_var[aparam] == 0){
                ctx->cand_var[cparam] = ctx->next_name;
                ctx->action_var[aparam] = ctx->next_name;
                ++ctx->next_name;

            }else if (ctx->cand_var[cparam] == 0
                        && ctx->action_var[aparam] > 0){
                ctx->cand_var[cparam] = ctx->action_var[aparam];

            }else if (ctx->cand_var[cparam] > 0
                        && ctx->action_var[aparam] == 0){
                ctx->action_var[aparam] = ctx->cand_var[cparam];

            }else if (ctx->cand_var[cparam] != ctx->action_var[aparam]){
                //  ctx->cand_var[cparam] > 0 && ctx->action_var[aparam] > 0
                if (ctx->cand_var[cparam] >= ctx->obj_offset
                        && ctx->action_var[aparam] >= ctx->obj_offset){
                    return 0;

                }else if (ctx->cand_var[cparam] >= ctx->obj_offset){
                    renameVars(ctx, ctx->action_var[aparam],
                                    ctx->cand_var[cparam]);

                }else{
                    renameVars(ctx, ctx->cand_var[cparam],
                                    ctx->action_var[aparam]);
                }
            }
        }else{
            int obj_id = ctx->obj_offset + a1->arg[i].obj;
            if (ctx->cand_var[cparam] == 0){
                ctx->cand_var[cparam] = obj_id;
            }else if (ctx->cand_var[cparam] >= ctx->obj_offset){
                if (ctx->cand_var[cparam] != obj_id)
                    return 0;
            }else{
                renameVars(ctx, ctx->cand_var[cparam], obj_id);
            }
        }
    }

    return 1;
}

/** Returns false if there are two action arguments that have assigned the
 *  same value, but their types are disjunct. */
static int argTypesAreValid(const ctx_action_t *ctx)
{
    const pddl_types_t *ts = &ctx->pddl->type;
    const pddl_action_t *action = ctx->action;
    const int *action_arg = ctx->action_var;

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

/** Returns true the inequalities in the precondition holds given the
 *  variable assignements */
static int inequalityPreHold(const ctx_action_t *ctx,
                             const pddl_cond_t *pre)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;

    if (pre == NULL)
        return 1;

    PDDL_COND_FOR_EACH_ATOM(pre, &it, a){
        if (a->neg && a->pred == ctx->pddl->pred.eq_pred){
            int v0 = atomArgValue(ctx, a, 0);
            int v1 = atomArgValue(ctx, a, 1);
            if (v0 == v1 && v0 != 0)
                return 0;
        }
    }
    return 1;
}

static int staticAtomHasEqArgs(const ctx_action_t *ctx, int pred_id,
                               int arg0, int arg1)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;

    PDDL_COND_FOR_EACH_ATOM(&ctx->pddl->init->cls, &it, a){
        if (!a->neg && a->pred == pred_id){
            if (a->arg[arg0].obj == a->arg[arg1].obj)
                return 1;
        }
    }
    return 0;
}

static int staticPreHold(const ctx_action_t *ctx, const pddl_cond_t *pre)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;

    if (pre == NULL)
        return 1;

    PDDL_COND_FOR_EACH_ATOM(pre, &it, a){
        if (!a->neg && pddlPredIsStatic(ctx->pddl->pred.pred + a->pred)){
            for (int i = 0; i < a->arg_size; ++i){
                int arg0 = atomArgValue(ctx, a, i);
                for (int j = i + 1; arg0 > 0 && j < a->arg_size; ++j){
                    if (arg0 == atomArgValue(ctx, a, j)){
                        if (!staticAtomHasEqArgs(ctx, a->pred, i, j))
                            return 0;
                    }
                }
            }
        }
    }

    return 1;
}

/** Returns true if c1 can be unified with a1 and c2 cand be unified with
 *  a2 so that a1 != a2, the types are compatible, and inequalities of
 *  ctx->action hold. */
static int isPairTooHeavyCand(ctx_action_t *ctx,
                              const pddl_cond_atom_t *c1,
                              const pddl_cond_atom_t *c2,
                              const ce_atom_t *a1,
                              const ce_atom_t *a2)
{
    CTX_RESET(ctx);
    if (!unifyAtoms(ctx, c1, a1->atom))
        return 0;

    // Empty counted variables because they can be bound to something else
    // now
    for (int i = 0; i < c2->arg_size; ++i){
        int param = c2->arg[i].param;
        if (param >= 0 && ctx->cand->param.param[param].is_counted_var)
            ctx->cand_var[param] = 0;
    }

    if (!unifyAtoms(ctx, c2, a2->atom))
        return 0;

    // If two variables has the same name, but the corresponding types are
    // disjunct, then we cannot unify the atoms
    if (!argTypesAreValid(ctx))
        return 0;

    // Check inequality predicates: we cannot assign the same name to two
    // arguments that cannot be same
    if (!inequalityPreHold(ctx, ctx->action->pre)
            || !inequalityPreHold(ctx, a1->pre)
            || !inequalityPreHold(ctx, a2->pre)){
        return 0;
    }

    // We unified two atoms, but we must check whether they differ
    if (atomsEq(ctx, a1->atom, a2->atom))
        return 0;

    // If exactly the same atoms are in the precondition, i.e.,
    // ((not a1) and (not a2)) is not satisfiable in the state where we
    // apply this action, then this action cannot increase the number of
    // facts in the resulting state.
    if (atomInPre(ctx, ctx->action->pre, a1->atom)
            || atomInPre(ctx, a1->pre, a1->atom)
            || atomInPre(ctx, ctx->action->pre, a2->atom)
            || atomInPre(ctx, a2->pre, a2->atom)){
        return 0;
    }

    // Check whether static preconditions are satisfiable
    if (!staticPreHold(ctx, ctx->action->pre)
            || !staticPreHold(ctx, a1->pre)
            || !staticPreHold(ctx, a2->pre)){
        return 0;
    }

    return 1;
}

/** Returns true if a1 and a2 cand be unified with some candidate atoms so
 *  that a1 != a2 and preconditions of ctx->action are satisfiable. */
static int isPairTooHeavy(ctx_action_t *ctx,
                          const ce_atom_t *a1,
                          const ce_atom_t *a2)
{
    const pddl_cond_atom_t *cand1, *cand2;

    FOR_EACH_CAND(ctx->cand, cand1){
        if (cand1->pred != a1->atom->pred)
            continue;
        FOR_EACH_CAND(ctx->cand, cand2){
            if (cand2->pred != a2->atom->pred)
                continue;
            if (isPairTooHeavyCand(ctx, cand1, cand2, a1, a2))
                return 1;
        }
    }
    return 0;
}

static int unifyAtomGroundedWithArgs(const pddl_t *pddl,
                                     const pddl_cond_atom_t *atom,
                                     const pddl_obj_id_t *atom_args,
                                     const pddl_lifted_mgroup_t *cand,
                                     const pddl_cond_atom_t *cand_atom,
                                     pddl_obj_id_t *arg)
{
    for (int i = 0; i < cand->param.param_size; ++i)
        arg[i] = PDDL_OBJ_ID_UNDEF;

    for (int i = 0; i < cand_atom->arg_size; ++i){
        pddl_obj_id_t atom_obj = atom->arg[i].obj;
        if (atom->arg[i].param >= 0 && atom_args != NULL)
            atom_obj = atom_args[atom->arg[i].param];
        ASSERT(atom_obj >= 0);
        int param = cand_atom->arg[i].param;
        pddl_obj_id_t obj = cand_atom->arg[i].obj;
        if (param >= 0
                && !pddlTypesObjHasType(&pddl->type,
                                        cand->param.param[param].type,
                                        atom_obj)){
            return 0;
        }
        if (obj != PDDL_OBJ_ID_UNDEF && obj != atom_obj){
            return 0;
        }else if (param >= 0 && !cand->param.param[param].is_counted_var){
            if (arg[param] == PDDL_OBJ_ID_UNDEF){
                arg[param] = atom_obj;
            }else if (arg[param] != atom_obj){
                return 0;
            }
        }
    }

    return 1;
}

static int unifyGroundedAtom(const pddl_t *pddl,
                             const pddl_cond_atom_t *atom,
                             const pddl_lifted_mgroup_t *cand,
                             const pddl_cond_atom_t *cand_atom,
                             pddl_obj_id_t *arg)
{
    return unifyAtomGroundedWithArgs(pddl, atom, NULL, cand, cand_atom, arg);
}

static int canUnifyAtomGroundedWithArgs(const pddl_t *pddl,
                                        const pddl_cond_atom_t *atom,
                                        const pddl_obj_id_t *atom_args,
                                        const pddl_lifted_mgroup_t *cand,
                                        const pddl_cond_atom_t *cand_atom,
                                        const pddl_obj_id_t *arg)
{
    for (int i = 0; i < cand_atom->arg_size; ++i){
        pddl_obj_id_t atom_obj = atom->arg[i].obj;
        if (atom->arg[i].param >= 0 && atom_args != NULL)
            atom_obj = atom_args[atom->arg[i].param];
        ASSERT(atom_obj >= 0);
        int param = cand_atom->arg[i].param;
        pddl_obj_id_t obj = cand_atom->arg[i].obj;
        if (param >= 0
                && !pddlTypesObjHasType(&pddl->type,
                                        cand->param.param[param].type,
                                        atom_obj)){
            return 0;
        }
        if (obj != PDDL_OBJ_ID_UNDEF && obj != atom_obj){
            return 0;
        }else if (param >= 0
                    && !cand->param.param[param].is_counted_var
                    && arg[param] != atom_obj){
            return 0;
        }
    }
    return 1;
}

static int canUnifyAtomArrGroundedWithArgs(const pddl_t *pddl,
                                           const pddl_lifted_mgroup_t *mg,
                                           const pddl_cond_arr_t *arr,
                                           const pddl_obj_id_t *arr_args)
{
    pddl_obj_id_t mg_arg[mg->param.param_size];
    for (int i = 0; i < mg->param.param_size; ++i)
        mg_arg[i] = PDDL_OBJ_ID_UNDEF;

    for (int i = 0; i < arr->size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(arr->cond[i], atom);
        const pddl_cond_atom_t *c;

        FOR_EACH_CAND(mg, c){
            if (c->pred != a->pred)
                continue;

            if (canUnifyAtomGroundedWithArgs(pddl, a, arr_args, mg, c, mg_arg))
                return 1;
        }
    }

    return 0;
}

/** Returns true if atom can be unified with cand_atom given bounding of
 *  cand arguments in arg. */
static int canUnifyGroundedAtom(const pddl_t *pddl,
                                const pddl_cond_atom_t *atom,
                                const pddl_lifted_mgroup_t *cand,
                                const pddl_cond_atom_t *cand_atom,
                                const pddl_obj_id_t *arg)
{
    return canUnifyAtomGroundedWithArgs(pddl, atom, NULL, cand, cand_atom, arg);
}

/** Returns true if the candidate atom is balanced with the given delete
 *  effect */
static int isBalancedWith(const ctx_action_t *ctx_in,
                          const pddl_cond_atom_t *catom,
                          const ce_atom_t *del_eff)
{
    CTX_PUSH(ctx_in, ctx);

    // Empty counted variables
    for (int i = 0; i < ctx.cand->param.param_size; ++i){
        if (ctx.cand->param.param[i].is_counted_var)
            ctx.cand_var[i] = 0;
    }

    for (int ai = 0; ai < catom->arg_size; ++ai){
        int cparam = catom->arg[ai].param;
        int dparam = del_eff->atom->arg[ai].param;
        int dobj = ctx.obj_offset + del_eff->atom->arg[ai].obj;
        if (ctx.cand->param.param[cparam].is_counted_var){
            // Counted variables can be instantiated with anything...
            if (dparam >= 0){
                if (ctx.action_var[dparam] == 0)
                    ctx.action_var[dparam] = ctx.next_name++;
                ctx.cand_var[cparam] = ctx.action_var[dparam];
            }else{
                ctx.cand_var[cparam] = dobj;
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
                if (ctx.action_var[dparam] > 0){
                    if (ctx.cand_var[cparam] != ctx.action_var[dparam]){
                        // Accounts for 1. and 2a.
                        return 0;
                    }

                }else{ // ctx.action_var[dparam] == 0
                    // Accounts for 2b.
                    return 0;
                }

            }else{
                if (ctx.cand_var[cparam] != dobj){
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
    if (atomInPre(&ctx, ctx.action->pre, del_eff->atom)
            || atomInPre(&ctx, del_eff->pre, del_eff->atom)){
        return 1;
    }

    // If we did not find a matching precondition, we report that the
    // delete effect cannot balance the add effect.
    return 0;
}


/** Restrict types of parameters so it is valid for all atoms. */
static void restrictParamTypes(const pddl_t *pddl, pddl_lifted_mgroup_t *mg)
{
    for (int ai = 0; ai < mg->cond.size; ++ai){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(mg->cond.cond[ai], atom);
        const pddl_pred_t *pred = pddl->pred.pred + a->pred;

        for (int i = 0; i < a->arg_size; ++i){
            if (a->arg[i].param >= 0){
                int mg_type = mg->param.param[a->arg[i].param].type;
                int pred_type = pred->param[i];
                if (pred_type != mg_type
                        && pddlTypesIsParent(&pddl->type, pred_type, mg_type)){
                    mg->param.param[a->arg[i].param].type = pred_type;
                }

                ASSERT(!pddlTypesAreDisjunct(&pddl->type, pred_type, mg_type));
            }
        }
    }
}

/** Add a new candidate refined from the given candidate and atom with
 *  specified parameters (-1 means counted variable, >=0 is an ID of the
 *  parameter. */
static void addRefinedCandidate(const pddl_t *pddl,
                                const pddl_lifted_mgroup_t *cand_in,
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
            param->type = pddl->pred.pred[atom->pred].param[i];
            param->is_counted_var = 1;
        }else{
            int type_cand = new_cand.param.param[atom_params[i]].type;
            int type_atom = pddl->pred.pred[new_atom->pred].param[i];
            if (pddlTypesAreDisjunct(&pddl->type, type_cand, type_atom)){
                pddlLiftedMGroupFree(&new_cand);
                return;
            }
            if (pddlTypesIsParent(&pddl->type, type_cand, type_atom))
                new_cand.param.param[atom_params[i]].type = type_atom;

            new_atom->arg[i].param = atom_params[i];
        }
    }
    pddlCondArrAdd(&new_cand.cond, &new_atom->cls);

    restrictParamTypes(pddl, &new_cand);

    pddlLiftedMGroupsAdd(refine, &new_cand);
    pddlLiftedMGroupFree(&new_cand);
}

/** Recursive function for refinement of candidates with the given delete
 *  effect */
static void refineCandidateWithDelEff(const ctx_action_t *ctx,
                                      const ce_atom_t *del_eff,
                                      int *del_eff_params,
                                      int del_eff_argi,
                                      int num_counted_vars,
                                      pddl_lifted_mgroups_t *refined)
{
    if (del_eff_argi == del_eff->atom->arg_size){
        if (atomInPre(ctx, ctx->action->pre, del_eff->atom)
                || atomInPre(ctx, del_eff->pre, del_eff->atom)){
            addRefinedCandidate(ctx->pddl, ctx->cand, del_eff->atom,
                                del_eff_params, refined);
        }
        return;
    }

    int del_eff_param, del_eff_obj;
    del_eff_param = del_eff->atom->arg[del_eff_argi].param;
    del_eff_obj = del_eff->atom->arg[del_eff_argi].obj;

    if (del_eff_param >= 0 && ctx->action_var[del_eff_param] == 0){
        if (num_counted_vars == 0){
            del_eff_params[del_eff_argi] = -1;
            CTX_PUSH(ctx, ctx2);
            if (ctx->action_var[del_eff_param] == 0)
                ctx2.action_var[del_eff_param] = ctx2.next_name++;

            refineCandidateWithDelEff(&ctx2, del_eff, del_eff_params,
                                      del_eff_argi + 1, num_counted_vars + 1,
                                      refined);
        }

    }else{
        int varname = ctx->obj_offset + del_eff_obj;
        if (del_eff_param >= 0)
            varname = ctx->action_var[del_eff_param];

        for (int ci = 0; ci < ctx->cand->param.param_size; ++ci){
            if (ctx->cand_var[ci] == varname
                    && !ctx->cand->param.param[ci].is_counted_var){
                del_eff_params[del_eff_argi] = ci;
                refineCandidateWithDelEff(ctx, del_eff, del_eff_params,
                                          del_eff_argi + 1, num_counted_vars,
                                          refined);
            }
        }

        if (num_counted_vars == 0){
            del_eff_params[del_eff_argi] = -1;
            refineCandidateWithDelEff(ctx, del_eff, del_eff_params,
                                      del_eff_argi + 1, num_counted_vars + 1,
                                      refined);
        }
    }
}

static void refineCandidate(const ctx_action_t *ctx,
                            pddl_lifted_mgroups_t *refined)
{
    pddl_cond_const_it_eff_t it;
    //pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;
    const pddl_cond_t *pre;

    PDDL_COND_FOR_EACH_DEL_EFF(ctx->action->eff, &it, a, pre){
        ASSERT(a->neg);
        if (!candHasPred(ctx->cand, a->pred)){
            int del_eff_params[a->arg_size];
            CE_ATOM(ce_a, pre, a);
            refineCandidateWithDelEff(ctx, &ce_a, del_eff_params,
                                      0, 0, refined);
        }
    }
}

static void candInstantiateParamWithObj(pddl_lifted_mgroup_t *dst,
                                        const pddl_lifted_mgroup_t *src,
                                        int param,
                                        pddl_obj_id_t obj)
{
    pddlLiftedMGroupInitCopy(dst, src);
    for (int ci = 0; ci < dst->cond.size; ++ci){
        pddl_cond_atom_t *a = PDDL_COND_CAST(dst->cond.cond[ci], atom);
        for (int i = 0; i < a->arg_size; ++i){
            if (a->arg[i].param == param){
                a->arg[i].param = -1;
                a->arg[i].obj = obj;
            }else if (a->arg[i].param > param){
                --a->arg[i].param;
            }
        }
    }

    // shift parameters
    for (int pi = param + 1; pi < dst->param.param_size; ++pi)
        dst->param.param[pi - 1] = dst->param.param[pi];
    --dst->param.param_size;
}

static void _tryInstantiateGivenInitTooHeavy(const pddl_t *pddl,
                                             const pddl_lifted_mgroup_t *cand,
                                             pddl_lifted_mgroups_t *lm,
                                             int param)
{
    ASSERT(!cand->param.param[param].is_counted_var);
    ASSERT(cand->param.param[param].type >= 0);
    const pddl_obj_id_t *obj;
    int obj_size;

    int type = cand->param.param[param].type;
    obj = pddlTypesObjsByType(&pddl->type, type, &obj_size);
    for (int oi = 0; oi < obj_size; ++oi){
        pddl_lifted_mgroup_t new_cand;
        candInstantiateParamWithObj(&new_cand, cand, param, obj[oi]);

        if (!pddlLiftedMGroupIsInitTooHeavy(&new_cand, pddl)){
            pddlLiftedMGroupsAdd(lm, &new_cand);
        }else{
            for (int next = param; next < new_cand.param.param_size; ++next){
                if (!new_cand.param.param[next].is_counted_var){
                    _tryInstantiateGivenInitTooHeavy(pddl, &new_cand, lm,
                                                     next);
                }
            }
        }

        pddlLiftedMGroupFree(&new_cand);
    }
}

static void tryInstantiateGivenInitTooHeavy(const pddl_t *pddl,
                                            const pddl_lifted_mgroup_t *cand,
                                            pddl_lifted_mgroups_t *mgroup)
{
    for (int i = 0; i < cand->param.param_size; ++i){
        if (!cand->param.param[i].is_counted_var)
            _tryInstantiateGivenInitTooHeavy(pddl, cand, mgroup, i);
    }
}

/** Returns true if the given add effect can be balanced by some delete
 *  effect. If it is not and refined != NULL, then new candidates are
 *  created. */
static int isAddEffBalanced(ctx_action_t *ctx,
                            const ce_atom_t *add_eff,
                            pddl_lifted_mgroups_t *refined)
{
    const pddl_cond_atom_t *cand_atom;

    FOR_EACH_CAND(ctx->cand, cand_atom){
        if (cand_atom->pred != add_eff->atom->pred)
            continue;

        CTX_RESET(ctx);
        if (!unifyAtoms(ctx, cand_atom, add_eff->atom)
                || !argTypesAreValid(ctx)
                || !inequalityPreHold(ctx, ctx->action->pre)
                || !inequalityPreHold(ctx, add_eff->pre)){
            continue;
        }

        pddl_cond_const_it_eff_t it_del;
        const pddl_cond_atom_t *del_eff;
        const pddl_cond_t *pre;
        int is_balanced = 0;
        PDDL_COND_FOR_EACH_DEL_EFF(ctx->action->eff, &it_del, del_eff, pre){
            ASSERT(del_eff->neg);
            if (is_balanced)
                break;
            // Consider only delete effects that are covered by the
            // candidate mgroup and that agree on the precondition with the
            // add effect it is suppose to cover.
            if (!candHasPred(ctx->cand, del_eff->pred)
                    || !pddlCondEq(pre, add_eff->pre)){
                continue;
            }

            CE_ATOM(ce_del_eff, pre, del_eff);
            const pddl_cond_atom_t *balance_cand;
            FOR_EACH_CAND(ctx->cand, balance_cand){
                if (balance_cand->pred != del_eff->pred)
                    continue;

                if (isBalancedWith(ctx, balance_cand, &ce_del_eff)){
                    is_balanced = 1;
                    break;
                }
            }
        }

        if (!is_balanced){
            if (refined != NULL)
                refineCandidate(ctx, refined);
            return 0;
        }
    }

    return 1;
}



void pddlLiftedMGroupInitEmpty(pddl_lifted_mgroup_t *dst)
{
    bzero(dst, sizeof(*dst));
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

static int cmpAtoms(const void *a, const void *b, void *_)
{
    const pddl_cond_t *c1 = *(const pddl_cond_t **)a;
    const pddl_cond_t *c2 = *(const pddl_cond_t **)b;
    const pddl_cond_atom_t *a1 = PDDL_COND_CAST(c1, atom);
    const pddl_cond_atom_t *a2 = PDDL_COND_CAST(c2, atom);
    return a1->pred - a2->pred;
}

void pddlLiftedMGroupSort(pddl_lifted_mgroup_t *m)
{
    if (m->cond.size <= 1)
        return;

    borSort(m->cond.cond, m->cond.size, sizeof(const pddl_cond_t *),
            cmpAtoms, NULL);
    if (m->param.param_size <= 1)
        return;

    int *remap_param = BOR_ALLOC_ARR(int, m->param.param_size);
    int *remap_param_inv = BOR_ALLOC_ARR(int, m->param.param_size);
    for (int i = 0; i < m->param.param_size; ++i){
        remap_param[i] = -1;
        remap_param_inv[i] = -1;
    }

    int num_non_counted = 0;
    for (int i = 0; i < m->param.param_size; ++i){
        if (!m->param.param[i].is_counted_var)
            ++num_non_counted;
    }

    int next = 0;
    int next_counted = num_non_counted;
    for (int i = 0; i < m->cond.size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(m->cond.cond[i], atom);
        for (int ai = 0; ai < a->arg_size; ++ai){
            if (a->arg[ai].obj != PDDL_OBJ_ID_UNDEF)
                continue;
            int param = a->arg[ai].param;
            if (m->param.param[param].is_counted_var){
                if (remap_param[param] < 0){
                    remap_param_inv[next_counted] = param;
                    remap_param[param] = next_counted++;
                }
            }else{
                if (remap_param[param] < 0){
                    remap_param_inv[next] = param;
                    remap_param[param] = next++;
                }
            }
        }
    }

#ifdef PDDL_DEBUG
    for (int i = 0; i < m->param.param_size; ++i){
        ASSERT(remap_param[i] >= 0);
    }
#endif /* PDDL_DEBUG */

    pddl_params_t param;
    pddlParamsInit(&param);
    for (int i = 0; i < m->param.param_size; ++i){
        ASSERT_RUNTIME(remap_param_inv[i] >= 0);
        pddl_param_t *p = pddlParamsAdd(&param);
        *p = m->param.param[remap_param_inv[i]];
    }
    pddlParamsFree(&m->param);
    m->param = param;

    for (int i = 0; i < m->cond.size; ++i){
        pddl_cond_atom_t *a = PDDL_COND_CAST(m->cond.cond[i], atom);
        for (int ai = 0; ai < a->arg_size; ++ai){
            if (a->arg[ai].param >= 0){
                ASSERT_RUNTIME(remap_param[a->arg[ai].param] >= 0);
                a->arg[ai].param = remap_param[a->arg[ai].param];
            }
        }
    }

    BOR_FREE(remap_param);
    BOR_FREE(remap_param_inv);
}

int pddlLiftedMGroupIsInitTooHeavy(const pddl_lifted_mgroup_t *cand,
                                   const pddl_t *pddl)
{
    pddl_obj_id_t arg[cand->param.param_size];
    pddl_cond_const_it_atom_t it1, it2;
    const pddl_cond_atom_t *a1, *a2;
    const pddl_cond_atom_t *cand1, *cand2;


    PDDL_COND_FOR_EACH_ATOM(&pddl->init->cls, &it1, a1){
        if (a1->neg)
            continue;
        FOR_EACH_CAND(cand, cand1){
            if (cand1->pred != a1->pred)
                continue;
            if (!unifyGroundedAtom(pddl, a1, cand, cand1, arg))
                continue;

            it2 = it1;
            PDDL_COND_FOR_EACH_CONT(&it2, a2){
                if (a2->neg)
                    continue;
                FOR_EACH_CAND(cand, cand2){
                    if (cand2->pred != a2->pred)
                        continue;
                    if (canUnifyGroundedAtom(pddl, a2, cand, cand2, arg))
                        return 1;
                }
            }
        }
    }

    return 0;
}

int pddlLiftedMGroupIsActionTooHeavy(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id)
{
    CTX_ACTION(ctx, pddl, action_id, cand);
    pddl_cond_const_it_eff_t it1, it2;
    const pddl_cond_atom_t *a1, *a2;
    const pddl_cond_t *pre1, *pre2;

    PDDL_COND_FOR_EACH_ADD_EFF(ctx.action->eff, &it1, a1, pre1){
        if (!candHasPred(cand, a1->pred))
            continue;
        CE_ATOM(ce_a1, pre1, a1);
        it2 = it1;
        PDDL_COND_FOR_EACH_ADD_EFF_CONT(&it2, a2, pre2){
            CE_ATOM(ce_a2, pre2, a2);
            if (candHasPred(cand, a2->pred)
                    && isPairTooHeavy(&ctx, &ce_a1, &ce_a2)){
                return 1;
            }
        }
    }

    return 0;
}

int pddlLiftedMGroupIsActionBalanced(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id,
                                     pddl_lifted_mgroups_t *refined)
{
    CTX_ACTION(ctx, pddl, action_id, cand);
    pddl_cond_const_it_eff_t it;
    const pddl_cond_atom_t *a;
    const pddl_cond_t *pre;

    PDDL_COND_FOR_EACH_ADD_EFF(ctx.action->eff, &it, a, pre){
        ASSERT(!a->neg);
        if (!candHasPred(cand, a->pred))
            continue;

        CE_ATOM(ce_a, pre, a);
        if (!isAddEffBalanced(&ctx, &ce_a, refined))
            return 0;
    }

    return 1;
}

int pddlLiftedMGroupIsGroundedConjTooHeavy(const pddl_lifted_mgroup_t *mg,
                                           const pddl_t *pddl,
                                           const pddl_cond_arr_t *arr,
                                           const pddl_obj_id_t *arr_args)
{
    pddl_obj_id_t mg_arg[mg->param.param_size];

    for (int i = 0; i < arr->size; ++i){
        const pddl_cond_atom_t *a1 = PDDL_COND_CAST(arr->cond[i], atom);
        ASSERT(!a1->neg);

        const pddl_cond_atom_t *m1;
        FOR_EACH_CAND(mg, m1){
            if (m1->pred != a1->pred)
                continue;
            if (!unifyAtomGroundedWithArgs(pddl, a1, arr_args, mg, m1, mg_arg))
                continue;

            for (int j = i + 1; j < arr->size; ++j){
                const pddl_cond_atom_t *a2 = PDDL_COND_CAST(arr->cond[j], atom);
                ASSERT(!a2->neg);
                const pddl_cond_atom_t *m2;
                FOR_EACH_CAND(mg, m2){
                    if (m2->pred != a2->pred)
                        continue;
                    if (canUnifyAtomGroundedWithArgs(pddl, a2, arr_args,
                                                     mg, m2, mg_arg)){
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

int pddlLiftedMGroupsIsGroundedConjTooHeavy(const pddl_lifted_mgroups_t *mgs,
                                            const pddl_t *pddl,
                                            const pddl_cond_arr_t *c,
                                            const pddl_obj_id_t *args)
{
    for (int i = 0; i < mgs->mgroup_size; ++i){
        if (pddlLiftedMGroupIsGroundedConjTooHeavy(mgs->mgroup + i, pddl,
                                                   c, args)){
            return 1;
        }
    }
    return 0;
}

int pddlLiftedMGroupIsDeleted(const pddl_lifted_mgroup_t *mg,
                              const pddl_t *pddl,
                              const pddl_cond_arr_t *pre,
                              const pddl_cond_arr_t *add_eff,
                              const pddl_cond_arr_t *del_eff,
                              const pddl_obj_id_t *args)
{
    if (canUnifyAtomArrGroundedWithArgs(pddl, mg, add_eff, args))
        return 0;

    pddl_obj_id_t mg_arg[mg->param.param_size];

    for (int di = 0; di < del_eff->size; ++di){
        const pddl_cond_atom_t *d = PDDL_COND_CAST(del_eff->cond[di], atom);
        const pddl_cond_atom_t *m;
        FOR_EACH_CAND(mg, m){
            if (m->pred != d->pred)
                continue;
            if (canUnifyAtomGroundedWithArgs(pddl, d, args, mg, m, mg_arg)
                    && atomInArrGroundedWithArgs(d, pre, args)){
                return 1;
            }
        }
    }
    return 0;
}

int pddlLiftedMGroupsAnyIsDeleted(const pddl_lifted_mgroups_t *mgs,
                                  const pddl_t *pddl,
                                  const pddl_cond_arr_t *pre,
                                  const pddl_cond_arr_t *add_eff,
                                  const pddl_cond_arr_t *del_eff,
                                  const pddl_obj_id_t *args)
{
    for (int i = 0; i < mgs->mgroup_size; ++i){
        if (pddlLiftedMGroupIsDeleted(mgs->mgroup + i, pddl,
                                      pre, add_eff, del_eff, args)){
            return 1;
        }
    }
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
                ASSERT(!pddlTypesAreDisjunct(&pddl->type, p->type,
                            pddl->pred.pred[atom->pred].param[j]));
                if (p->is_counted_var){
                    fprintf(fout, " C%d:%s",
                            param_id, pddl->type.type[p->type].name);
                }else{
                    fprintf(fout, " V%d:%s",
                            param_id, pddl->type.type[p->type].name);
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
    pddlLiftedMGroupSort(add);
}

void pddlLiftedMGroupsAddInst(pddl_lifted_mgroups_t *lm,
                              const pddl_lifted_mgroup_t *lmg,
                              const pddl_obj_id_t *args)
{
    pddlLiftedMGroupsAdd(lm, lmg);

    pddl_lifted_mgroup_t *mg = lm->mgroup + lm->mgroup_size - 1;
    pddl_cond_atom_t *a;
    FOR_EACH_CAND(mg, a){
        for (int ai = 0; ai < a->arg_size; ++ai){
            if (a->arg[ai].param >= 0 && args[a->arg[ai].param] >= 0){
                a->arg[ai].obj = args[a->arg[ai].param];
                a->arg[ai].param = -1;
            }
        }
    }

    int remap_param[mg->param.param_size];
    int idx = 0;
    for (int i = 0; i < mg->param.param_size; ++i){
        if (args[i] >= 0){
            remap_param[i] = -1;
        }else{
            mg->param.param[idx] = mg->param.param[i];
            remap_param[i] = idx++;
        }
    }
    mg->param.param_size = idx;

    for (int i = 0; i < mg->cond.size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(mg->cond.cond[i], atom);
        for (int ai = 0; ai < a->arg_size; ++ai){
            if (a->arg[ai].param >= 0)
                a->arg[ai].param = remap_param[a->arg[ai].param];
        }
    }
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
        if (cmp == 0)
            cmp = m1->param.param[i].type - m2->param.param[i].type;
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

void pddlLiftedMGroupsExtractGoalAware(pddl_lifted_mgroups_t *dst,
                                       const pddl_lifted_mgroups_t *src,
                                       const pddl_t *pddl)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;

    for (int i = 0; i < src->mgroup_size; ++i){
        const pddl_lifted_mgroup_t *mg = src->mgroup + i;
        pddl_obj_id_t arg[mg->param.param_size];

        PDDL_COND_FOR_EACH_ATOM(pddl->goal, &it, a){
            ASSERT(!a->neg);
            const pddl_cond_atom_t *c;
            FOR_EACH_CAND(mg, c){
                if (c->pred != a->pred)
                    continue;

                if (unifyGroundedAtom(pddl, a, mg, c, arg)){
                    pddlLiftedMGroupsAddInst(dst, mg, arg);
                }
            }
        }
    }
    pddlLiftedMGroupsSortAndUniq(dst);
}

static void initialCandidates(const pddl_t *pddl, pddl_lifted_mgroups_t *lm)
{
    for (int pred_id = 0; pred_id < pddl->pred.pred_size; ++pred_id){
        const pddl_pred_t *pred = pddl->pred.pred + pred_id;
        if (pddlPredIsStatic(pred) || pred_id == pddl->pred.eq_pred)
            continue;

        pddl_lifted_mgroup_t m;

        pddlLiftedMGroupInitCandFromPred(&m, pred, -1);
        pddlLiftedMGroupsAdd(lm, &m);
        pddlLiftedMGroupFree(&m);

        for (int i = 0; i < pred->param_size; ++i){
            pddlLiftedMGroupInitCandFromPred(&m, pred, i);
            pddlLiftedMGroupsAdd(lm, &m);
            pddlLiftedMGroupFree(&m);
        }
    }
}

static int isSingleFact(const pddl_lifted_mgroup_t *cand)
{
    if (cand->cond.size != 1)
        return 0;

    const pddl_cond_atom_t *a = PDDL_COND_CAST(cand->cond.cond[0], atom);
    for (int i = 0; i < a->arg_size; ++i){
        if (cand->param.param[a->arg[i].param].is_counted_var)
            return 0;
    }
    return 1;
}

void pddlLiftedMGroupsInfer(const pddl_t *pddl, pddl_lifted_mgroups_t *lm)
{
    pddl_lifted_mgroups_t cands[2] = { 0 };

    // TODO: Parametrize number of candidates

    initialCandidates(pddl, cands + 0);
    int cur = 0;
    for (cur = 0; cands[cur].mgroup_size > 0; cur = (cur + 1) % 2){
        int next = (cur + 1) % 2;

        pddlLiftedMGroupsSortAndUniq(cands + cur);
        pddlLiftedMGroupsInit(cands + next);
        for (int cid = 0; cid < cands[cur].mgroup_size; ++cid){
            const pddl_lifted_mgroup_t *cand = cands[cur].mgroup + cid;
            if (isInitTooHeavyForCountedVars(cand, pddl)){
                // Quickly throw away candidates that cannot be mgroups
                // under any circumstances
                continue;
            }

            int proved = 1;
            for (int ai = 0; ai < pddl->action.action_size; ++ai){
                if (pddlLiftedMGroupIsActionTooHeavy(cand, pddl, ai)
                        || !pddlLiftedMGroupIsActionBalanced(cand, pddl, ai,
                                                             cands + next)){
                    proved = 0;
                    break;
                }
            }

            if (proved && !isSingleFact(cand)){
                if (pddlLiftedMGroupIsInitTooHeavy(cand, pddl)){
                    tryInstantiateGivenInitTooHeavy(pddl, cand, lm);
                }else{
                    pddlLiftedMGroupsAdd(lm, cand);
                }
            }
        }

        pddlLiftedMGroupsFree(cands + cur);
    }
    pddlLiftedMGroupsFree(cands + cur);

    pddlLiftedMGroupsSortAndUniq(lm);
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
