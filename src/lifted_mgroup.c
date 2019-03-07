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

// TODO: If there is only one possible object of certain type, treat is as
//       a object everywhere.
// TODO: Merge candidates with proved mgroups if possible (e.g., when types
//       of variables are subtypes)

#include <boruvka/sort.h>
#include <boruvka/hfunc.h>
#include <boruvka/htable.h>
#include <boruvka/extarr.h>
#include "pddl/pddl.h"
#include "pddl/lifted_mgroup.h"
#include "assert.h"

struct candidate {
    int id;
    pddl_lifted_mgroup_t mgroup;
    bor_htable_key_t hash;
    bor_list_t htable;
};
typedef struct candidate candidate_t;

struct candidates {
    bor_htable_t *htable; /*!< Hash table of all candidates */
    bor_extarr_t *cand; /*!< Extensible array holding candidates */
    int cand_size; /*!< Number of candidates */
    int next;

    const pddl_t *pddl;
    pddl_lifted_mgroups_infer_config_t cfg;
};
typedef struct candidates candidates_t;

struct ce_atom {
    const pddl_cond_t *pre;
    const pddl_cond_atom_t *atom;
};
typedef struct ce_atom ce_atom_t;

#define CE_ATOM(NAME, PRE, ATOM) \
    ce_atom_t NAME = { (PRE), (ATOM) }

struct unify_action_ctx {
    const pddl_t *pddl;
    const pddl_params_t *action_param;
    const pddl_params_t *cand_param;
    int *action_arg;
    int *cand_arg;
    int next_name;
};
typedef struct unify_action_ctx unify_action_ctx_t;

#define UNIFY_ACTION_CTX(NAME, PDDL, ACTION_PARAMS, CAND_PARAMS) \
    int __action_arg_##NAME[(ACTION_PARAMS)->param_size]; \
    for (int __i = 0; __i < (ACTION_PARAMS)->param_size; ++__i) \
        __action_arg_##NAME[__i] = -1; \
    int __cand_arg_##NAME[(CAND_PARAMS)->param_size]; \
    for (int __i = 0; __i < (CAND_PARAMS)->param_size; ++__i) \
        __cand_arg_##NAME[__i] = -1; \
    unify_action_ctx_t NAME = { (PDDL), (ACTION_PARAMS), (CAND_PARAMS), \
        __action_arg_##NAME, __cand_arg_##NAME, (PDDL)->obj.obj_size }

#define UNIFY_ACTION_CTX_PUSH(NAME, SRC) \
    int __action_arg_##NAME[(SRC)->action_param->param_size]; \
    memcpy(__action_arg_##NAME, (SRC)->action_arg, \
            sizeof(int) * (SRC)->action_param->param_size); \
    int __cand_arg_##NAME[(SRC)->cand_param->param_size]; \
    memcpy(__cand_arg_##NAME, (SRC)->cand_arg, \
            sizeof(int) * (SRC)->cand_param->param_size); \
    unify_action_ctx_t NAME = { \
        (SRC)->pddl, (SRC)->action_param, (SRC)->cand_param, \
        __action_arg_##NAME, __cand_arg_##NAME, (SRC)->next_name }


#define FOR_EACH_CAND(CAND, C) \
    for (int ___i = 0; \
            ___i < (CAND)->cond.size \
                && ((C) = PDDL_COND_CAST((CAND)->cond.cond[___i], atom)); \
                ++___i)


/** Compare function for array of pddl_lifted_mgroup_t */
static int cmpLiftedMGroups(const void *a, const void *b, void *_);
/** Returns true if the two mutex groups are equal */
static int mgroupsEq(const pddl_lifted_mgroup_t *m1,
                     const pddl_lifted_mgroup_t *m2);
/** Returns hash key of the mutex group */
static bor_htable_key_t mgroupHash(const pddl_lifted_mgroup_t *m);

/** Initialize pool of candidates */
static void candidatesInit(candidates_t *p,
                           const pddl_t *pddl,
                           const pddl_lifted_mgroups_infer_config_t *cfg);
/** Free allocated memory */
static void candidatesFree(candidates_t *p);
/** Adds the given mcandidate. If m is already there -1 is returned,
 *  otherwise 0 is returned. */
static int candidatesAdd(candidates_t *p, const pddl_lifted_mgroup_t *m);
/** Returns true if no more candidates are available */
static int candidatesEmpty(const candidates_t *p);
/** Returns next candidate from the pool */
static const pddl_lifted_mgroup_t *candidatesNext(candidates_t *p);


/** Returns true if the candidate contains atom of the specified predicate */
static int candHasPred(const pddl_lifted_mgroup_t *cand, int pred);
/** Returns false if there are two action arguments that have assigned the
 *  same value, but their types are disjunct. */
static int actionArgTypesAreValid(const pddl_t *pddl,
                                  const pddl_params_t *params,
                                  const int *args);
/** Returns value corresponding to the specified argument */
static int atomArg(const pddl_cond_atom_t *atom, int argi, const int *args);
/** Returns true if the atoms are equal under the given variable assignment */
static int atomsEqual(const pddl_cond_atom_t *atom1,
                      const pddl_cond_atom_t *atom2,
                      const int *args);
/** Returns true if the exactly same atom can be found in conj */
static int equalAtomIn(const pddl_cond_atom_t *atom,
                       const pddl_cond_t *conj,
                       const int *args);
static int equalAtomInArr(const pddl_cond_atom_t *atom,
                          const pddl_cond_arr_t *arr,
                          const int *args);
/** Returns true the inequalities in the conjuction hold given the
 *  bound arguments */
static int inequalitiesHold(const pddl_t *pddl,
                            const pddl_cond_t *pre,
                            const int *args);
/** Returns true if static preconditions are not violated with the given
 *  arguments */
static int staticPreHold(const pddl_t *pddl,
                         const pddl_cond_t *pre,
                         const int *args);

/** Unify fact (grounded with fact_arg) with the candidate atom */
static int unifyFact(const pddl_t *pddl,
                     const pddl_cond_atom_t *fact,
                     const pddl_obj_id_t *fact_arg,
                     const pddl_params_t *cand_params,
                     const pddl_cond_atom_t *cand_atom,
                     pddl_obj_id_t *cand_arg);
/** Returns true if fact (grounded with fact_arg) can be unified with the
 *  given candidate atom and arguments. */
static int canUnifyFact(const pddl_t *pddl,
                        const pddl_cond_atom_t *fact,
                        const pddl_obj_id_t *fact_arg,
                        const pddl_params_t *cand_params,
                        const pddl_cond_atom_t *cand_atom,
                        const pddl_obj_id_t *cand_arg);
/** Returns true if the atoms are compatible, i.e., they are the same
 *  predicate and arguments have matching types/objects */
static int atomsAreCompatible(const pddl_t *pddl,
                              const pddl_cond_atom_t *a1,
                              const pddl_params_t *a1_params,
                              const pddl_cond_atom_t *a2,
                              const pddl_params_t *a2_params);
/** Unify action's atom with the candidate atom */
static int unifyActionAtom(unify_action_ctx_t *ctx,
                           const pddl_cond_atom_t *action_atom,
                           const pddl_cond_atom_t *cand_atom);


/** Returns true if the conjuction of grounded atoms is too heavy for the
 *  candidate mutex group */
static int isGroundedCondArrTooHeavy(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     const pddl_cond_arr_t *arr,
                                     const pddl_obj_id_t *arr_args);
static int isGroundedConjTooHeavy(const pddl_lifted_mgroup_t *cand,
                                  const pddl_t *pddl,
                                  const pddl_cond_t *conj);
/** Returns the if the initial state is too heavy */
static int isInitTooHeavy(const pddl_lifted_mgroup_t *cand, const pddl_t *pddl);
/** Returns true if the intial state is too heavy for the counted variables */
static int isInitTooHeavyForCountedVars(const pddl_lifted_mgroup_t *cand_in,
                                        const pddl_t *pddl);
/** Returns true if the action is too heavy */
static int isActionTooHeavy(const pddl_lifted_mgroup_t *cand,
                            const pddl_t *pddl,
                            const pddl_action_t *action,
                            candidates_t *cands);

/** Returns true if the action has balanced all add effects.
 *  If it has not, the candidate is refined and added to cands. */
static int isActionBalanced(const pddl_lifted_mgroup_t *cand,
                            const pddl_t *pddl,
                            const pddl_action_t *action,
                            candidates_t *cands);

/** TODO **/
static void refineTooHeavyInit(const pddl_t *pddl,
                               const pddl_cond_atom_t *a1,
                               const pddl_cond_atom_t *a2,
                               const pddl_lifted_mgroup_t *cand,
                               const pddl_cond_atom_t *cand_atom1,
                               const pddl_cond_atom_t *cand_atom2,
                               candidates_t *cands);

/** TODO **/
/** Refine candidate cand by extending it with more atoms so that add
 *  effects are balanced. */
static void refineExtend(const unify_action_ctx_t *ctx,
                         const pddl_action_t *action,
                         const pddl_lifted_mgroup_t *cand,
                         candidates_t *cands);

/** Refine candidate cand by changing types of candidate variables so that
 *  atom and cand_atom cannot be unified. */
static void refineTypes(const pddl_types_t *ts,
                        const pddl_params_t *params,
                        const pddl_cond_atom_t *atom,
                        const pddl_lifted_mgroup_t *cand,
                        const pddl_cond_atom_t *cand_atom,
                        candidates_t *cands);

/** Refine candidate cand by changing counted variables to non-counted
 *  variables so that a1 and a2 cannot be unified with cand_atom1 and
 *  cand_atom2. */
static void refineVariables(const pddl_cond_atom_t *a1,
                            const pddl_cond_atom_t *a2,
                            const pddl_lifted_mgroup_t *cand,
                            const pddl_cond_atom_t *cand_atom1,
                            const pddl_cond_atom_t *cand_atom2,
                            candidates_t *cands);

/** Shortcut for refineTypes() + refineVariables() for the case when
 *  candidate is too-heavy */
static void refineTooHeavy(const pddl_types_t *ts,
                           const pddl_params_t *params,
                           const pddl_cond_atom_t *a1,
                           const pddl_cond_atom_t *a2,
                           const pddl_lifted_mgroup_t *cand,
                           const pddl_cond_atom_t *cand_atom1,
                           const pddl_cond_atom_t *cand_atom2,
                           candidates_t *cands);

/** Shortcut for refinement of candidates for which the action has
 *  unbalanced add effect */
static void refineActionUnbalanced(const unify_action_ctx_t *ctx,
                                   const pddl_action_t *action,
                                   const pddl_cond_atom_t *add_eff,
                                   const pddl_lifted_mgroup_t *cand,
                                   const pddl_cond_atom_t *cand_add_eff,
                                   candidates_t *cands);


/** Try to instantiate some non-counted variables in cand to have at most
 *  one matching atom in conj. Successfuly instantiate mgroups are added to
 *  mgroup. */
static void removeHeavinessByInst(const pddl_t *pddl,
                                  const pddl_cond_t *conj,
                                  const pddl_lifted_mgroup_t *cand,
                                  pddl_lifted_mgroups_t *mgroup);

/** Generates the initial candidates */
static void initialCandidates(const pddl_t *pddl,
                              const pddl_lifted_mgroups_infer_config_t *cfg,
                              candidates_t *cands);




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

static bor_htable_key_t mgroupHash(const pddl_lifted_mgroup_t *m)
{
    int *buf;
    int bufsize;

    bufsize = m->param.param_size * 2;
    for (int i = 0; i < m->cond.size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(m->cond.cond[i], atom);
        bufsize += 1 + a->arg_size;
    }

    buf = BOR_ALLOC_ARR(int, bufsize);

    for (int i = 0; i < m->param.param_size; ++i){
        buf[2 * i] = m->param.param[i].type;
        buf[2 * i + 1] = m->param.param[i].is_counted_var;
    }

    int ins = 2 * m->param.param_size;
    for (int i = 0; i < m->cond.size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(m->cond.cond[i], atom);
        buf[ins++] = a->pred;
        for (int ai = 0; ai < a->arg_size; ++ai){
            if (a->arg[ai].param >= 0){
                buf[ins++] = a->arg[ai].param;
            }else{
                buf[ins++] = a->arg[ai].obj * -1;
            }
        }
    }

    ASSERT(ins == bufsize);
    bor_htable_key_t hash = borCityHash_64(buf, bufsize * sizeof(int));

    BOR_FREE(buf);
    return hash;
}

static bor_htable_key_t htableHash(const bor_list_t *k, void *_)
{
    candidate_t *m = BOR_LIST_ENTRY(k, candidate_t, htable);
    return m->hash;
}

static int htableEq(const bor_list_t *k1, const bor_list_t *k2, void *_)
{
    candidate_t *m1 = BOR_LIST_ENTRY(k1, candidate_t, htable);
    candidate_t *m2 = BOR_LIST_ENTRY(k2, candidate_t, htable);
    return mgroupsEq(&m1->mgroup, &m2->mgroup);
}

static void candidatesInit(candidates_t *p,
                           const pddl_t *pddl,
                           const pddl_lifted_mgroups_infer_config_t *cfg)
{
    candidate_t mg;

    bzero(p, sizeof(*p));
    p->htable = borHTableNew(htableHash, htableEq, p);

    bzero(&mg, sizeof(mg));
    p->cand = borExtArrNew(sizeof(mg), NULL, &mg);
    p->next = 0;

    p->cfg = *cfg;
    p->pddl = pddl;
}

static void candidatesFree(candidates_t *p)
{
    for (int i = 0; i < p->cand_size; ++i){
        candidate_t *m = borExtArrGet(p->cand, i);
        pddlLiftedMGroupFree(&m->mgroup);
    }

    borHTableDel(p->htable);
    borExtArrDel(p->cand);
}

static int candidatesAdd(candidates_t *p, const pddl_lifted_mgroup_t *m)
{
    if (p->cand_size >= p->cfg.max_candidates)
        return -1;

    candidate_t *el = borExtArrGet(p->cand, p->cand_size);
    el->mgroup = *m;
    el->hash = mgroupHash(m);

    bor_list_t *ins = borHTableInsertUnique(p->htable, &el->htable);
    if (ins == NULL){
        pddlLiftedMGroupInitCopy(&el->mgroup, m);
        el->id = p->cand_size;
        ++p->cand_size;
        return 0;

    }else{
        return -1;
    }
}

static int candidatesEmpty(const candidates_t *p)
{
    return p->cand_size == p->next;
}

static const pddl_lifted_mgroup_t *candidatesNext(candidates_t *p)
{
    candidate_t *cand;

    cand = borExtArrGet(p->cand, p->next++);
    return &cand->mgroup;
}



static int candHasPred(const pddl_lifted_mgroup_t *cand, int pred)
{
    for (int i = 0; i < cand->cond.size; ++i){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(cand->cond.cond[i], atom);
        if (a->pred == pred)
            return 1;
    }
    return 0;
}

/** TODO **/
static int candHasAddEff(const pddl_lifted_mgroup_t *cand,
                         const pddl_cond_t *eff)
{
    pddl_cond_const_it_eff_t it;
    const pddl_cond_atom_t *a, *c;
    const pddl_cond_t *pre;

    PDDL_COND_FOR_EACH_ADD_EFF(eff, &it, a, pre){
        FOR_EACH_CAND(cand, c){
            if (a->pred == c->pred)
                return 1;
        }
    }

    return 0;
}

/** TODO **/
static int candHasCountedVar(const pddl_lifted_mgroup_t *cand)
{
    for (int i = 0; i < cand->param.param_size; ++i){
        if (cand->param.param[i].is_counted_var)
            return 1;
    }
    return 0;
}

static int actionArgTypesAreValid(const pddl_t *pddl,
                                  const pddl_params_t *params,
                                  const int *args)
{
    const pddl_types_t *ts = &pddl->type;

    for (int i = 0; i < params->param_size; ++i){
        if (args[i] < 0)
            continue;
        int type1 = params->param[i].type;

        for (int j = i + 1; j < params->param_size; ++j){
            int type2 = params->param[j].type;
            if (args[i] == args[j] && pddlTypesAreDisjunct(ts, type1, type2))
                return 0;
        }
    }

    return 1;
}

static int atomArg(const pddl_cond_atom_t *atom, int argi, const int *args)
{
    int param = atom->arg[argi].param;
    if (param >= 0)
        return args[param];
    return atom->arg[argi].obj;
}

static int atomsEqual(const pddl_cond_atom_t *atom1,
                      const pddl_cond_atom_t *atom2,
                      const int *args)
{
    if (atom1->pred != atom2->pred)
        return 0;

    for (int ai = 0; ai < atom1->arg_size; ++ai){
        if (atomArg(atom1, ai, args) != atomArg(atom2, ai, args))
            return 0;
    }
    return 1;
}

static int equalAtomIn(const pddl_cond_atom_t *atom,
                       const pddl_cond_t *conj,
                       const int *args)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a2;

    if (conj == NULL)
        return 0;

    PDDL_COND_FOR_EACH_ATOM(conj, &it, a2){
        if (!a2->neg && atomsEqual(a2, atom, args))
            return 1;
    }
    return 0;
}

static int equalAtomInArr(const pddl_cond_atom_t *atom,
                          const pddl_cond_arr_t *arr,
                          const int *args)
{
    if (arr == NULL)
        return 0;

    for (int i = 0; i < arr->size; ++i){
        const pddl_cond_atom_t *a2 = PDDL_COND_CAST(arr->cond[i], atom);
        if (!a2->neg && atomsEqual(a2, atom, args))
            return 1;
    }
    return 0;
}

static int inequalitiesHold(const pddl_t *pddl,
                            const pddl_cond_t *pre,
                            const int *args)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;

    if (pre == NULL)
        return 1;

    PDDL_COND_FOR_EACH_ATOM(pre, &it, a){
        if (a->neg && a->pred == pddl->pred.eq_pred){
            int v0 = atomArg(a, 0, args);
            int v1 = atomArg(a, 1, args);
            if (v0 == v1 && v0 >= 0)
                return 0;
        }
    }
    return 1;
}

static int staticAtomHasEqArgs(const pddl_t *pddl, int pred_id,
                               int arg0, int arg1)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;

    PDDL_COND_FOR_EACH_ATOM(&pddl->init->cls, &it, a){
        if (!a->neg && a->pred == pred_id){
            if (a->arg[arg0].obj == a->arg[arg1].obj)
                return 1;
        }
    }
    return 0;
}

static int staticPreHold(const pddl_t *pddl,
                         const pddl_cond_t *pre,
                         const int *args)
{
    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;

    if (pre == NULL)
        return 1;

    PDDL_COND_FOR_EACH_ATOM(pre, &it, a){
        if (!a->neg && pddlPredIsStatic(pddl->pred.pred + a->pred)){
            for (int i = 0; i < a->arg_size; ++i){
                int arg0 = atomArg(a, i, args);
                for (int j = i + 1; arg0 >= 0 && j < a->arg_size; ++j){
                    if (arg0 == atomArg(a, j, args)){
                        if (!staticAtomHasEqArgs(pddl, a->pred, i, j))
                            return 0;
                    }
                }
            }
        }
    }

    return 1;
}


static int _unifyFact(const pddl_t *pddl,
                      const pddl_cond_atom_t *fact,
                      const pddl_obj_id_t *fact_arg,
                      const pddl_params_t *cand_params,
                      const pddl_cond_atom_t *cand_atom,
                      pddl_obj_id_t *cand_arg)
{
    if (fact->pred != cand_atom->pred)
        return 0;

    ASSERT(fact->arg_size == cand_atom->arg_size);
    for (int i = 0; i < fact->arg_size; ++i){
        pddl_obj_id_t fact_obj = fact->arg[i].obj;
        if (fact_arg != NULL)
            fact_obj = atomArg(fact, i, fact_arg);
        ASSERT(fact_obj >= 0);

        int param = cand_atom->arg[i].param;
        pddl_obj_id_t obj = cand_atom->arg[i].obj;
        if (param >= 0){
            if (!pddlTypesObjHasType(&pddl->type,
                                     cand_params->param[param].type,
                                     fact_obj)){
                return 0;
            }

            if (!cand_params->param[param].is_counted_var){
                if (cand_arg[param] == PDDL_OBJ_ID_UNDEF){
                    cand_arg[param] = fact_obj;
                }else if (cand_arg[param] != fact_obj){
                    return 0;
                }
            }

        }else{
            ASSERT(obj != PDDL_OBJ_ID_UNDEF);
            if (obj != fact_obj)
                return 0;
        }
    }

    return 1;
}

static int unifyFact(const pddl_t *pddl,
                     const pddl_cond_atom_t *fact,
                     const pddl_obj_id_t *fact_arg,
                     const pddl_params_t *cand_params,
                     const pddl_cond_atom_t *cand_atom,
                     pddl_obj_id_t *cand_arg)
{
    for (int i = 0; i < cand_params->param_size; ++i)
        cand_arg[i] = PDDL_OBJ_ID_UNDEF;
    return _unifyFact(pddl, fact, fact_arg, cand_params, cand_atom, cand_arg);
}

static int canUnifyFact(const pddl_t *pddl,
                        const pddl_cond_atom_t *fact,
                        const pddl_obj_id_t *fact_arg,
                        const pddl_params_t *cand_params,
                        const pddl_cond_atom_t *cand_atom,
                        const pddl_obj_id_t *cand_arg)
{
    pddl_obj_id_t args[cand_params->param_size];
    memcpy(args, cand_arg, sizeof(pddl_obj_id_t) * cand_params->param_size);
    return _unifyFact(pddl, fact, fact_arg, cand_params, cand_atom, args);
}

static int atomsAreCompatible(const pddl_t *pddl,
                              const pddl_cond_atom_t *a1,
                              const pddl_params_t *a1_params,
                              const pddl_cond_atom_t *a2,
                              const pddl_params_t *a2_params)
{
    if (a1->pred != a2->pred)
        return 0;

    for (int i = 0; i < a1->arg_size; ++i){
        if (a1->arg[i].param >= 0 && a2->arg[i].param >= 0){
            int a1type = a1_params->param[a1->arg[i].param].type;
            int a2type = a2_params->param[a2->arg[i].param].type;
            if (pddlTypesAreDisjunct(&pddl->type, a1type, a2type))
                return 0;

        }else if (a1->arg[i].param >= 0){ // && a2->arg[i].obj >= 0
            int a1type = a1_params->param[a1->arg[i].param].type;
            if (!pddlTypesObjHasType(&pddl->type, a1type, a2->arg[i].obj))
                return 0;

        }else if (a2->arg[i].param >= 0){ // && a1->arg[i].obj >= 0
            int a2type = a2_params->param[a2->arg[i].param].type;
            if (!pddlTypesObjHasType(&pddl->type, a2type, a1->arg[i].obj))
                return 0;

        }else{ // a1->arg[i].obj >= 0 && a2->arg[i].obj >= 0
            if (a1->arg[i].obj != a2->arg[i].obj)
                return 0;
        }
    }
    return 1;
}

static void renameArgs(unify_action_ctx_t *ctx, int from, int to)
{
    for (int i = 0; i < ctx->cand_param->param_size; ++i){
        if (ctx->cand_arg[i] == from)
            ctx->cand_arg[i] = to;
    }
    for (int i = 0; i < ctx->action_param->param_size; ++i){
        if (ctx->action_arg[i] == from)
            ctx->action_arg[i] = to;
    }
}

static int unifyActionAtom(unify_action_ctx_t *ctx,
                           const pddl_cond_atom_t *action_atom,
                           const pddl_cond_atom_t *cand_atom)
{
    if (!atomsAreCompatible(ctx->pddl,
                            cand_atom, ctx->cand_param,
                            action_atom, ctx->action_param)){
        return 0;
    }

    // Empty counted variables because they can be bound to something else
    // now
    for (int i = 0; i < cand_atom->arg_size; ++i){
        int param = cand_atom->arg[i].param;
        if (param >= 0 && ctx->cand_param->param[param].is_counted_var)
            ctx->cand_arg[param] = -1;
    }

    ASSERT(action_atom->arg_size == cand_atom->arg_size);
    for (int i = 0; i < cand_atom->arg_size; ++i){
        int aparam = action_atom->arg[i].param;
        int cparam = cand_atom->arg[i].param;

        if (aparam >= 0 && cparam >= 0){
            if (ctx->cand_arg[cparam] < 0 && ctx->action_arg[aparam] < 0){
                // Neither of cand and action parameters are bound. So bind
                // them to the same name
                ctx->cand_arg[cparam] = ctx->next_name;
                ctx->action_arg[aparam] = ctx->next_name;
                ++ctx->next_name;

            }else if (ctx->cand_arg[cparam] < 0){
                // Only cand param is not set, so copy the same name from
                // the action parameter
                ctx->cand_arg[cparam] = ctx->action_arg[aparam];

            }else if (ctx->action_arg[aparam] < 0){
                // Only action param is not set
                ctx->action_arg[aparam] = ctx->cand_arg[cparam];

            }else if (ctx->cand_arg[cparam] != ctx->action_arg[aparam]){
                // Both parameters are set, but they are different
                if (ctx->cand_arg[cparam] < ctx->pddl->obj.obj_size
                        && ctx->action_arg[aparam] < ctx->pddl->obj.obj_size){
                    // Both are set to an object that is different, which
                    // means that unification is not possible
                    return 0;

                }else if (ctx->cand_arg[cparam] < ctx->pddl->obj.obj_size){
                    // Candidate parameter is bound to an object, so
                    // propagate the same object to all parameters
                    renameArgs(ctx, ctx->action_arg[aparam],
                                    ctx->cand_arg[cparam]);

                }else{
                    renameArgs(ctx, ctx->cand_arg[cparam],
                                    ctx->action_arg[aparam]);
                }
            }

        }else if (cparam >= 0){
            int obj_id = action_atom->arg[i].obj;
            if (ctx->cand_arg[cparam] < 0){
                ctx->cand_arg[cparam] = obj_id;
            }else if (ctx->cand_arg[cparam] < ctx->pddl->obj.obj_size){
                if (ctx->cand_arg[cparam] != obj_id)
                    return 0;
            }else{
                renameArgs(ctx, ctx->cand_arg[cparam], obj_id);
            }

        }else if (aparam >= 0){
            int obj_id = cand_atom->arg[i].obj;
            if (ctx->action_arg[aparam] < 0){
                ctx->action_arg[aparam] = obj_id;
            }else if (ctx->action_arg[aparam] < ctx->pddl->obj.obj_size){
                if (ctx->action_arg[aparam] != obj_id)
                    return 0;
            }else{
                renameArgs(ctx, ctx->action_arg[aparam], obj_id);
            }

        }else{
            if (action_atom->arg[i].obj != cand_atom->arg[i].obj)
                return 0;
        }
    }

    return 1;
}

/*** HEAVINESS TEST ***/
/** Returns true if the effect pair can be unified with the candidate atom
 *  pair. **/
static int canUnifyEffPair(const pddl_t *pddl,
                           const pddl_action_t *action,
                           const ce_atom_t *eff1,
                           const ce_atom_t *eff2,
                           const pddl_params_t *cand_params,
                           const pddl_cond_atom_t *cand1,
                           const pddl_cond_atom_t *cand2)
{
    UNIFY_ACTION_CTX(ctx, pddl, &action->param, cand_params);
    if (!unifyActionAtom(&ctx, eff1->atom, cand1)
            || !unifyActionAtom(&ctx, eff2->atom, cand2)){
        return 0;
    }

    // If two variables has the same name, but the corresponding types are
    // disjunct, then we cannot unify the atoms
    if (!actionArgTypesAreValid(pddl, ctx.action_param, ctx.action_arg))
        return 0;

    // Check inequality predicates: we cannot assign the same name to two
    // arguments that cannot be same
    if (!inequalitiesHold(pddl, action->pre, ctx.action_arg)
            || !inequalitiesHold(pddl, eff1->pre, ctx.action_arg)
            || !inequalitiesHold(pddl, eff2->pre, ctx.action_arg)){
        return 0;
    }

    // We unified two atoms, but we must check whether they differ
    if (atomsEqual(eff1->atom, eff2->atom, ctx.action_arg))
        return 0;

    // If exactly the same atoms are in the precondition, i.e.,
    // ((not a1) and (not a2)) is not satisfiable in the state where we
    // apply this action, then this action cannot increase the number of
    // facts in the resulting state.
    if (equalAtomIn(eff1->atom, action->pre, ctx.action_arg)
            || equalAtomIn(eff1->atom, eff1->pre, ctx.action_arg)
            || equalAtomIn(eff2->atom, action->pre, ctx.action_arg)
            || equalAtomIn(eff2->atom, eff2->pre, ctx.action_arg)){
        return 0;
    }

    // Check whether static preconditions are satisfiable
    if (!staticPreHold(pddl, action->pre, ctx.action_arg)
            || !staticPreHold(pddl, eff1->pre, ctx.action_arg)
            || !staticPreHold(pddl, eff2->pre, ctx.action_arg)){
        return 0;
    }

    return 1;
}

static int isInitExactlyOne(const pddl_t *pddl,
                            const pddl_lifted_mgroup_t *cand,
                            candidates_t *cands)
{
    pddl_obj_id_t arg[cand->param.param_size];
    const pddl_cond_atom_t *cand1, *cand2;
    pddl_cond_const_it_atom_t it1, it2;
    const pddl_cond_atom_t *a1, *a2;
    int unified = 0;

    PDDL_COND_FOR_EACH_ATOM(&pddl->init->cls, &it1, a1){
        if (a1->neg)
            continue;

        FOR_EACH_CAND(cand, cand1){
            if (cand1->pred != a1->pred)
                continue;
            if (!unifyFact(pddl, a1, NULL, &cand->param, cand1, arg))
                continue;

            unified = 1;

            it2 = it1;
            PDDL_COND_FOR_EACH_CONT(&it2, a2){
                if (a2->neg)
                    continue;
                FOR_EACH_CAND(cand, cand2){
                    if (cand2->pred != a2->pred)
                        continue;
                    if (canUnifyFact(pddl, a2, NULL,
                                     &cand->param, cand2, arg)){
                        refineTooHeavy(&pddl->type, NULL, a1, a2,
                                       cand, cand1, cand2, cands);
                        return 0;
                    }
                }
            }
        }
    }

    return unified;
}

static int isGroundedCondArrTooHeavy(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     const pddl_cond_arr_t *arr,
                                     const pddl_obj_id_t *arr_args)
{
    pddl_obj_id_t arg[cand->param.param_size];
    const pddl_cond_atom_t *cand1, *cand2;

    for (int i = 0; i < arr->size; ++i){
        const pddl_cond_atom_t *a1 = PDDL_COND_CAST(arr->cond[i], atom);
        if (a1->neg)
            continue;
        FOR_EACH_CAND(cand, cand1){
            if (cand1->pred != a1->pred)
                continue;
            if (!unifyFact(pddl, a1, arr_args, &cand->param, cand1, arg))
                continue;

            for (int j = i + 1; j < arr->size; ++j){
                const pddl_cond_atom_t *a2 = PDDL_COND_CAST(arr->cond[j], atom);
                if (a2->neg)
                    continue;
                FOR_EACH_CAND(cand, cand2){
                    if (cand2->pred != a2->pred)
                        continue;
                    if (canUnifyFact(pddl, a2, arr_args,
                                     &cand->param, cand2, arg)){
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

static int isGroundedConjTooHeavy(const pddl_lifted_mgroup_t *cand,
                                  const pddl_t *pddl,
                                  const pddl_cond_t *conj)
{
    pddl_cond_arr_t arr;

    pddlCondArrInit(&arr);

    pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;
    PDDL_COND_FOR_EACH_ATOM(conj, &it, a){
        if (a->neg)
            continue;
        pddlCondArrAdd(&arr, &a->cls);
    }
    int ret = isGroundedCondArrTooHeavy(cand, pddl, &arr, NULL);
    pddlCondArrFree(&arr);
    return ret;
}

static int isInitTooHeavy(const pddl_lifted_mgroup_t *cand, const pddl_t *pddl)
{
    return isGroundedConjTooHeavy(cand, pddl, &pddl->init->cls);
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
    // TODO: Generalize for any counted variable (not just atoms having
    //       only counted variables
    int ret = 0;

    pddl_lifted_mgroup_t cand;
    selectOnlyAtomsWithCountedVars(&cand, cand_in);

    if (cand.cond.size > 0)
        ret = isInitTooHeavy(&cand, pddl);

    pddlLiftedMGroupFree(&cand);
    return ret;
}

static int isActionTooHeavy(const pddl_lifted_mgroup_t *cand,
                            const pddl_t *pddl,
                            const pddl_action_t *action,
                            candidates_t *cands)
{
    pddl_cond_const_it_eff_t it1, it2;
    const pddl_cond_atom_t *a1, *a2, *cand1, *cand2;
    const pddl_cond_t *pre1, *pre2;

    PDDL_COND_FOR_EACH_ADD_EFF(action->eff, &it1, a1, pre1){
        CE_ATOM(ce_a1, pre1, a1);

        FOR_EACH_CAND(cand, cand1){
            if (cand1->pred != a1->pred)
                continue;

            it2 = it1;
            PDDL_COND_FOR_EACH_ADD_EFF_CONT(&it2, a2, pre2){
                CE_ATOM(ce_a2, pre2, a2);

                FOR_EACH_CAND(cand, cand2){
                    if (cand2->pred != a2->pred)
                        continue;

                    if (canUnifyEffPair(pddl, action, &ce_a1, &ce_a2,
                                        &cand->param, cand1, cand2)){
                        refineTooHeavy(&pddl->type, &action->param, a1, a2,
                                       cand, cand1, cand2, cands);
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

static int isAnyActionTooHeavy(const pddl_t *pddl,
                               const pddl_lifted_mgroup_t *cand,
                               candidates_t *cands)
{
    for (int ai = 0; ai < pddl->action.action_size; ++ai){
        const pddl_action_t *a = pddl->action.action + ai;
        if (isActionTooHeavy(cand, pddl, a, cands)){
            return 1;
        }
    }

    return 0;
}

/*** BALANCE TEST ***/
// TODO: Rename to unifyActionEff()
static int unifyActionAddEff(unify_action_ctx_t *ctx,
                             const pddl_action_t *action,
                             const ce_atom_t *eff,
                             const pddl_cond_atom_t *cand_atom)
{
    return unifyActionAtom(ctx, eff->atom, cand_atom)
                && actionArgTypesAreValid(ctx->pddl, &action->param,
                                          ctx->action_arg)
                && inequalitiesHold(ctx->pddl, action->pre, ctx->action_arg)
                && inequalitiesHold(ctx->pddl, eff->pre, ctx->action_arg);
}

/** Returns true if the add effect is balanced by the given delete effect **/
static int isAddEffBalancedWith(const unify_action_ctx_t *ctx_in,
                                const pddl_action_t *action,
                                const ce_atom_t *del_eff,
                                const pddl_cond_atom_t *cand_atom)
{
    if (!atomsAreCompatible(ctx_in->pddl,
                            cand_atom, ctx_in->cand_param,
                            del_eff->atom, ctx_in->action_param)){
        return 0;
    }

    UNIFY_ACTION_CTX_PUSH(ctx, ctx_in);

    // Empty counted variables
    for (int i = 0; i < ctx.cand_param->param_size; ++i){
        if (ctx.cand_param->param[i].is_counted_var)
            ctx.cand_arg[i] = -1;
    }

    ASSERT(cand_atom->pred == del_eff->atom->pred);
    ASSERT(cand_atom->arg_size == del_eff->atom->arg_size);
    for (int ai = 0; ai < cand_atom->arg_size; ++ai){
        int cparam = cand_atom->arg[ai].param;
        int dparam = del_eff->atom->arg[ai].param;
        if (cparam >= 0 && dparam >= 0){
            if (ctx.cand_param->param[cparam].is_counted_var){
                // check that the type of the candidate's param is not too
                // narrow, i.e., if it may be possible to instantiate
                // the delete effect with something outside the candidate's
                // type
                int dtype = ctx.action_param->param[dparam].type;
                int ctype = ctx.cand_param->param[cparam].type;
                if (dtype != ctype
                        && pddlTypesIsParent(&ctx.pddl->type, ctype, dtype)){
                    return 0;
                }

                if (ctx.action_arg[dparam] < 0)
                    ctx.action_arg[dparam] = ctx.next_name++;
                ctx.cand_arg[cparam] = ctx.action_arg[dparam];

            }else{
                if (ctx.cand_arg[cparam] != ctx.action_arg[dparam]
                        || ctx.action_arg[dparam] < 0){
                    return 0;
                }
            }

        }else if (cparam >= 0){
            int dobj = del_eff->atom->arg[ai].obj;
            if (ctx.cand_param->param[cparam].is_counted_var){
                int ctype = ctx.cand_param->param[cparam].type;
                if (!pddlTypesObjHasType(&ctx.pddl->type, ctype, dobj))
                    return 0;
                ctx.cand_arg[cparam] = dobj;

            }else{
                if (ctx.cand_arg[cparam] != dobj)
                    return 0;
            }

        }else if (dparam >= 0){
            int cobj = cand_atom->arg[ai].obj;
            if (ctx.action_arg[dparam] != cobj)
                return 0;

        }else{
            int cobj = cand_atom->arg[ai].obj;
            int dobj = del_eff->atom->arg[ai].obj;
            if (cobj != dobj)
                return 0;
        }
    }

    // Now we have assigned names to action variables and we must check
    // that there is a precondition exactly matching the delete effect so
    // we can be sure that the delete effect is present in the state the
    // action is applied on, i.e., that the delete effect really balances
    // the add effect.
    if (equalAtomIn(del_eff->atom, action->pre, ctx.action_arg)
            || equalAtomIn(del_eff->atom, del_eff->pre, ctx.action_arg)){
        return 1;
    }

    // If we did not find a matching precondition, we report that the
    // delete effect cannot balance the add effect.
    return 0;
}

static int isAddEffBalanced(const unify_action_ctx_t *ctx,
                            const pddl_action_t *action,
                            const ce_atom_t *add_eff,
                            const pddl_lifted_mgroup_t *cand)
{
    pddl_cond_const_it_eff_t it_del;
    const pddl_cond_atom_t *del_eff_atom;
    const pddl_cond_t *pre;
    PDDL_COND_FOR_EACH_DEL_EFF(action->eff, &it_del, del_eff_atom, pre){
        ASSERT(del_eff_atom->neg);
        // Consider only delete effects that agree on the precondition with
        // the add effect it is suppose to cover.
        if (!pddlCondEq(add_eff->pre, pre))
            continue;
        CE_ATOM(del_eff, pre, del_eff_atom);

        const pddl_cond_atom_t *cand_atom;
        FOR_EACH_CAND(cand, cand_atom){
            if (cand_atom->pred != del_eff_atom->pred)
                continue;
            if (isAddEffBalancedWith(ctx, action, &del_eff, cand_atom))
                return 1;
        }
    }

    return 0;
}
                            

static int isActionBalanced(const pddl_lifted_mgroup_t *cand,
                            const pddl_t *pddl,
                            const pddl_action_t *action,
                            candidates_t *cands)
{
    pddl_cond_const_it_eff_t it;
    const pddl_cond_atom_t *a;
    const pddl_cond_t *pre;
    PDDL_COND_FOR_EACH_ADD_EFF(action->eff, &it, a, pre){
        ASSERT(!a->neg);
        CE_ATOM(add_eff, pre, a);

        const pddl_cond_atom_t *cand_atom;
        FOR_EACH_CAND(cand, cand_atom){
            if (cand_atom->pred != a->pred)
                continue;

            UNIFY_ACTION_CTX(ctx, pddl, &action->param, &cand->param);
            if (unifyActionAddEff(&ctx, action, &add_eff, cand_atom)){
                if (!isAddEffBalanced(&ctx, action, &add_eff, cand)){
                    refineActionUnbalanced(&ctx, action, a,
                                           cand, cand_atom, cands);
                    return 0;
                }
            }
        }
    }

    return 1;
}


/*** REFINEMENT ***/
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
                                candidates_t *cands)
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
    pddlLiftedMGroupSort(&new_cand);
    candidatesAdd(cands, &new_cand);
    pddlLiftedMGroupFree(&new_cand);
}

/** Recursive function for refinement of candidates with the given delete
 *  effect */
static void refineCandidateWithDelEff(const unify_action_ctx_t *ctx,
                                      const pddl_action_t *action,
                                      const pddl_lifted_mgroup_t *cand,
                                      const ce_atom_t *del_eff,
                                      int *del_eff_params,
                                      int del_eff_argi,
                                      int num_counted_vars,
                                      candidates_t *cands)
{
    if (del_eff_argi == del_eff->atom->arg_size){
        if (equalAtomIn(del_eff->atom, action->pre, ctx->action_arg)
                || equalAtomIn(del_eff->atom, del_eff->pre, ctx->action_arg)){
            addRefinedCandidate(ctx->pddl, cand, del_eff->atom,
                                del_eff_params, cands);
        }
        return;
    }

    int del_eff_param = del_eff->atom->arg[del_eff_argi].param;
    int del_eff_obj = del_eff->atom->arg[del_eff_argi].obj;

    if (del_eff_param >= 0 && ctx->action_arg[del_eff_param] < 0){
        // TODO
        //if (num_counted_vars == 0){
            del_eff_params[del_eff_argi] = -1;
            UNIFY_ACTION_CTX_PUSH(ctx2, ctx);
            if (ctx->action_arg[del_eff_param] < 0)
                ctx2.action_arg[del_eff_param] = ctx2.next_name++;

            refineCandidateWithDelEff(&ctx2, action, cand,
                                      del_eff, del_eff_params,
                                      del_eff_argi + 1, num_counted_vars + 1,
                                      cands);
        //}

    }else{
        int arg = del_eff_obj;
        if (del_eff_param >= 0)
            arg = ctx->action_arg[del_eff_param];

        for (int ci = 0; ci < cand->param.param_size; ++ci){
            if (ctx->cand_arg[ci] == arg
                    && !cand->param.param[ci].is_counted_var){
                del_eff_params[del_eff_argi] = ci;
                refineCandidateWithDelEff(ctx, action, cand,
                                          del_eff, del_eff_params,
                                          del_eff_argi + 1, num_counted_vars,
                                          cands);
            }
        }

        //if (num_counted_vars == 0){
            del_eff_params[del_eff_argi] = -1;
            refineCandidateWithDelEff(ctx, action, cand,
                                      del_eff, del_eff_params,
                                      del_eff_argi + 1, num_counted_vars + 1,
                                      cands);
        //}
    }
}

static void refineExtend(const unify_action_ctx_t *ctx,
                         const pddl_action_t *action,
                         const pddl_lifted_mgroup_t *cand,
                         candidates_t *cands)
{
    if (cands == NULL || cands->cand_size >= cands->cfg.max_candidates)
        return;

    pddl_cond_const_it_eff_t it;
    //pddl_cond_const_it_atom_t it;
    const pddl_cond_atom_t *a;
    const pddl_cond_t *pre;

    PDDL_COND_FOR_EACH_DEL_EFF(action->eff, &it, a, pre){
        ASSERT(a->neg);
        if (!candHasPred(cand, a->pred)){
            int del_eff_params[a->arg_size];
            CE_ATOM(ce_a, pre, a);
            refineCandidateWithDelEff(ctx, action, cand,
                                      &ce_a, del_eff_params, 0, 0, cands);
        }
    }
}

// TODO: Refactor with refineCandidateWithDelEff()
// Allow more than one counted variable
static void refineCandidateWithAddEff(const unify_action_ctx_t *ctx,
                                      const pddl_action_t *action,
                                      const pddl_lifted_mgroup_t *cand,
                                      const ce_atom_t *atom,
                                      int *atom_params,
                                      int atom_argi,
                                      int num_counted_vars,
                                      candidates_t *cands)
{
    if (atom_argi == atom->atom->arg_size){
        //fprintf(stderr, "NEW CAND\n");
        addRefinedCandidate(ctx->pddl, cand, atom->atom,
                            atom_params, cands);
        return;
    }

    int atom_param = atom->atom->arg[atom_argi].param;
    int atom_obj = atom->atom->arg[atom_argi].obj;

    if (atom_param >= 0 && ctx->action_arg[atom_param] < 0){
        // TODO
        //if (num_counted_vars == 0){
            atom_params[atom_argi] = -1;
            UNIFY_ACTION_CTX_PUSH(ctx2, ctx);
            if (ctx->action_arg[atom_param] < 0)
                ctx2.action_arg[atom_param] = ctx2.next_name++;

            refineCandidateWithAddEff(&ctx2, action, cand,
                                      atom, atom_params,
                                      atom_argi + 1, num_counted_vars + 1,
                                      cands);
        //}

    }else{
        int arg = atom_obj;
        if (atom_param >= 0)
            arg = ctx->action_arg[atom_param];

        for (int ci = 0; ci < cand->param.param_size; ++ci){
            if (ctx->cand_arg[ci] == arg
                    && !cand->param.param[ci].is_counted_var){
                atom_params[atom_argi] = ci;
                refineCandidateWithAddEff(ctx, action, cand,
                                          atom, atom_params,
                                          atom_argi + 1, num_counted_vars,
                                          cands);
            }
        }

        //if (num_counted_vars == 0){
            atom_params[atom_argi] = -1;
            refineCandidateWithAddEff(ctx, action, cand,
                                      atom, atom_params,
                                      atom_argi + 1, num_counted_vars + 1,
                                      cands);
        //}
    }
}

static void refineExtendProvedWithAddEff(const unify_action_ctx_t *ctx,
                                         const pddl_action_t *action,
                                         const pddl_lifted_mgroup_t *cand,
                                         candidates_t *cands)
{
    pddl_cond_const_it_eff_t it;
    const pddl_cond_atom_t *a;
    const pddl_cond_t *pre;

    PDDL_COND_FOR_EACH_ADD_EFF(action->eff, &it, a, pre){
        ASSERT(a->neg);
        CE_ATOM(ce_a, pre, a);
        int eff_params[a->arg_size];
        //fprintf(stderr, "Extend with %s\n",
        //        ctx->pddl->pred.pred[a->pred].name);
        refineCandidateWithAddEff(ctx, action, cand,
                                  &ce_a, eff_params, 0, 0, cands);
    }
}

static void refineExtendProved(const pddl_t *pddl,
                               const pddl_action_t *action,
                               const pddl_lifted_mgroup_t *cand,
                               candidates_t *cands)
{
    if (cands == NULL || cands->cand_size >= cands->cfg.max_candidates)
        return;

    pddl_cond_const_it_eff_t it;
    const pddl_cond_atom_t *a, *c;
    const pddl_cond_t *pre;

    PDDL_COND_FOR_EACH_DEL_EFF(action->eff, &it, a, pre){
        ASSERT(a->neg);
        CE_ATOM(ce_a, pre, a);

        FOR_EACH_CAND(cand, c){
            if (c->pred != a->pred)
                continue;
            UNIFY_ACTION_CTX(ctx, pddl, &action->param, &cand->param);
            // TODO: Describe the following condition
            if (unifyActionAddEff(&ctx, action, &ce_a, c)
                    && (equalAtomIn(a, action->pre, ctx.action_arg)
                            || equalAtomIn(a, pre, ctx.action_arg))
                    && !candHasAddEff(cand, action->eff)){
                refineExtendProvedWithAddEff(&ctx, action, cand, cands);
            }
        }
    }
}

static void addCandidateWithChangedParamType(const pddl_lifted_mgroup_t *cand,
                                             int param,
                                             int type,
                                             candidates_t *cands)
{
    pddl_lifted_mgroup_t new_cand;
    pddlLiftedMGroupInitCopy(&new_cand, cand);
    new_cand.param.param[param].type = type;
    candidatesAdd(cands, &new_cand);
    /*
       pddlLiftedMGroupPrint(pddl, cand, stderr);
       pddlLiftedMGroupPrint(pddl, &new_cand, stderr);
       fprintf(stderr, "^^^ %d\n", param_id);
       fflush(stderr);
     */

    pddlLiftedMGroupFree(&new_cand);
}

static void refineTypes(const pddl_types_t *ts,
                        const pddl_params_t *params,
                        const pddl_cond_atom_t *atom,
                        const pddl_lifted_mgroup_t *cand,
                        const pddl_cond_atom_t *cand_atom,
                        candidates_t *cands)
{
    if (cands == NULL
            || cands->cand_size >= cands->cfg.max_candidates
            || !cands->cfg.use_type_refinement){
        return;
    }

    for (int argi = 0; argi < atom->arg_size; ++argi){
        if (cand_atom->arg[argi].obj >= 0)
            continue;

        int cparam = cand_atom->arg[argi].param;
        int ctype = cand->param.param[cparam].type;

        int aparam = atom->arg[argi].param;
        pddl_obj_id_t aobj = atom->arg[argi].obj;
        int atype = -1;
        const pddl_type_t *at;
        if (aparam >= 0){
            atype = params->param[aparam].type;
            at = ts->type + atype;
        }

        for (int type = 0; type < ts->type_size; ++type){
            if (type == ctype || !pddlTypesIsParent(ts, type, ctype))
                continue;
            const pddl_type_t *t = ts->type + type;
            if ((atype >= 0
                        && pddlTypesAreDisjunct(ts, type, atype)
                        && (t->parent == ctype || t->parent == at->parent))
                    || (aobj >= 0 && !pddlTypesObjHasType(ts, type, aobj))){
                addCandidateWithChangedParamType(cand, cparam, type, cands);
            }
        }
    }
}

static void countedVariables(const pddl_lifted_mgroup_t *cand,
                             const pddl_cond_atom_t *atom,
                             bor_iset_t *vars)
{
    for (int i = 0; i < atom->arg_size; ++i){
        if (atom->arg[i].param >= 0
                && cand->param.param[atom->arg[i].param].is_counted_var){
            borISetAdd(vars, atom->arg[i].param);
        }
    }
}
static void refineVariables(const pddl_cond_atom_t *a1,
                            const pddl_cond_atom_t *a2,
                            const pddl_lifted_mgroup_t *cand,
                            const pddl_cond_atom_t *cand_atom1,
                            const pddl_cond_atom_t *cand_atom2,
                            candidates_t *cands)
{
    //fprintf(stderr, "refineVariables\n");
    // TODO: Add variable -> object refinement
    // TODO: parametrize
    if (cands == NULL
            || cands->cand_size >= cands->cfg.max_candidates){
        return;
    }

    BOR_ISET(relevant_params);

    // Collect counted variables present in both cand_atom1 and cand_atom2
    BOR_ISET(counted_vars2);
    countedVariables(cand, cand_atom1, &relevant_params);
    countedVariables(cand, cand_atom2, &counted_vars2);
    borISetIntersect(&relevant_params, &counted_vars2);
    borISetFree(&counted_vars2);
    //fprintf(stderr, "Cs: %d\n", borISetSize(&relevant_params));

    // If a1 and a2 differ in a argument corresonding to counted variable,
    // then we can try to change this variable to non-counted variable
    int counted_var;
    BOR_ISET_FOR_EACH(&relevant_params, counted_var){
        //fprintf(stderr, "C: %d\n", counted_var);
        for (int ai1 = 0; ai1 < cand_atom1->arg_size; ++ai1){
            if (cand_atom1->arg[ai1].param != counted_var)
                continue;
            for (int ai2 = 0; ai2 < cand_atom2->arg_size; ++ai2){
                if (cand_atom2->arg[ai2].param != counted_var)
                    continue;

                if (a1->arg[ai1].obj != a2->arg[ai2].obj
                        || a1->arg[ai1].param != a2->arg[ai2].param){
                    pddl_lifted_mgroup_t new_cand;
                    pddlLiftedMGroupInitCopy(&new_cand, cand);
                    ASSERT(new_cand.param.param[counted_var].is_counted_var);
                    new_cand.param.param[counted_var].is_counted_var = 0;
                    candidatesAdd(cands, &new_cand);
                    pddlLiftedMGroupFree(&new_cand);
                }
            }
        }
    }

    borISetFree(&relevant_params);
}

static void refineTooHeavy(const pddl_types_t *ts,
                           const pddl_params_t *params,
                           const pddl_cond_atom_t *a1,
                           const pddl_cond_atom_t *a2,
                           const pddl_lifted_mgroup_t *cand,
                           const pddl_cond_atom_t *cand_atom1,
                           const pddl_cond_atom_t *cand_atom2,
                           candidates_t *cands)
{
    //fprintf(stderr, "refineTooHeavy\n");
    if (cands == NULL || cands->cand_size >= cands->cfg.max_candidates){
        return;
    }

    refineTypes(ts, params, a1, cand, cand_atom1, cands);
    refineTypes(ts, params, a2, cand, cand_atom2, cands);
    refineVariables(a1, a2, cand, cand_atom1, cand_atom2, cands);
}

static void refineActionUnbalanced(const unify_action_ctx_t *ctx,
                                   const pddl_action_t *action,
                                   const pddl_cond_atom_t *add_eff,
                                   const pddl_lifted_mgroup_t *cand,
                                   const pddl_cond_atom_t *cand_add_eff,
                                   candidates_t *cands)
{
    if (cands == NULL || cands->cand_size >= cands->cfg.max_candidates){
        return;
    }

    refineExtend(ctx, action, cand, cands);
    refineTypes(&ctx->pddl->type, &action->param, add_eff,
                cand, cand_add_eff, cands);
}

static void refineTooHeavyInit(const pddl_t *pddl,
                               const pddl_cond_atom_t *a1,
                               const pddl_cond_atom_t *a2,
                               const pddl_lifted_mgroup_t *cand,
                               const pddl_cond_atom_t *cand_atom1,
                               const pddl_cond_atom_t *cand_atom2,
                               candidates_t *cands)
{
    if (cands == NULL)
        return;

    refineTypes(&pddl->type, NULL, a1, cand, cand_atom1, cands);
    refineTypes(&pddl->type, NULL, a2, cand, cand_atom2, cands);

    pddl_obj_id_t arg[cand->param.param_size];
    for (int i = 0; i < cand->param.param_size; ++i)
        arg[i] = PDDL_OBJ_ID_UNDEF;

    ASSERT(a1->pred == cand_atom1->pred);
    ASSERT(a1->arg_size == cand_atom1->arg_size);
    ASSERT(a2->pred == cand_atom2->pred);
    ASSERT(a2->arg_size == cand_atom2->arg_size);
    for (int ai = 0; ai < cand_atom1->arg_size; ++ai){
        ASSERT(a1->arg[ai].obj >= 0);
        if (cand_atom1->arg[ai].param >= 0){
            ASSERT(arg[cand_atom1->arg[ai].param] == PDDL_OBJ_ID_UNDEF
                    || arg[cand_atom1->arg[ai].param] == a1->arg[ai].obj);
            arg[cand_atom1->arg[ai].param] = a1->arg[ai].obj;
        }
    }

    for (int ai = 0; ai < cand_atom2->arg_size; ++ai){
        ASSERT(a2->arg[ai].obj >= 0);
        if (cand_atom2->arg[ai].param >= 0){
            if (arg[cand_atom2->arg[ai].param] == PDDL_OBJ_ID_UNDEF){
                arg[cand_atom2->arg[ai].param] = a2->arg[ai].obj;
            }else if (arg[cand_atom2->arg[ai].param] != a2->arg[ai].obj){
                ASSERT(cand->param.param[cand_atom2->arg[ai].param].is_counted_var);
                pddl_lifted_mgroup_t m;
                pddlLiftedMGroupInitCopy(&m, cand);
                m.param.param[cand_atom2->arg[ai].param].is_counted_var = 0;
                candidatesAdd(cands, &m);
                pddlLiftedMGroupFree(&m);
            }
        }
    }
}





/*** PARTIAL INSTANTIATION ***/
/** Set all parameters param to object obj and removes parameter param **/
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

static void _removeHeavinessByInst(const pddl_t *pddl,
                                   const pddl_cond_t *conj,
                                   const pddl_lifted_mgroup_t *cand,
                                   int param,
                                   pddl_lifted_mgroups_t *lm)
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

        if (!isGroundedConjTooHeavy(&new_cand, pddl, conj)){
            pddlLiftedMGroupsAdd(lm, &new_cand);
        }else{
            for (int next = param; next < new_cand.param.param_size; ++next){
                if (!new_cand.param.param[next].is_counted_var)
                    _removeHeavinessByInst(pddl, conj, &new_cand, next, lm);
            }
        }

        pddlLiftedMGroupFree(&new_cand);
    }
}

static void removeHeavinessByInst(const pddl_t *pddl,
                                  const pddl_cond_t *conj,
                                  const pddl_lifted_mgroup_t *cand,
                                  pddl_lifted_mgroups_t *mgroup)
{
    for (int i = 0; i < cand->param.param_size; ++i){
        if (!cand->param.param[i].is_counted_var)
            _removeHeavinessByInst(pddl, conj, cand, i, mgroup);
    }
}


static void _initialCandidates(const pddl_t *pddl,
                               const pddl_lifted_mgroup_t *m,
                               int param,
                               candidates_t *cands)
{
    pddl_lifted_mgroup_t cand;

    pddlLiftedMGroupInitCopy(&cand, m);
    if (param == cand.param.param_size - 1){
        cand.param.param[param].is_counted_var = 0;
        candidatesAdd(cands, &cand);
        cand.param.param[param].is_counted_var = 1;
        candidatesAdd(cands, &cand);
    }else{
        cand.param.param[param].is_counted_var = 0;
        _initialCandidates(pddl, &cand, param + 1, cands);
        cand.param.param[param].is_counted_var = 1;
        _initialCandidates(pddl, &cand, param + 1, cands);
    }
    pddlLiftedMGroupFree(&cand);
}


static void initialCandidates(const pddl_t *pddl,
                              const pddl_lifted_mgroups_infer_config_t *cfg,
                              candidates_t *cands)
{
    candidates_t init_cands;

    candidatesInit(&init_cands, pddl, cfg);

    // TODO: Parametrize
    for (int pred_id = 0; pred_id < pddl->pred.pred_size; ++pred_id){
        const pddl_pred_t *pred = pddl->pred.pred + pred_id;
        if (pddlPredIsStatic(pred) || pred_id == pddl->pred.eq_pred)
            continue;

        pddl_lifted_mgroup_t m;

        pddlLiftedMGroupInitCandFromPred(&m, pred, -1);
        for (int i = 0; i < m.param.param_size; ++i)
            m.param.param[i].is_counted_var = 1;
        candidatesAdd(&init_cands, &m);
        candidatesAdd(cands, &m);
        //pddlLiftedMGroupPrint(pddl, &m, stderr);
        pddlLiftedMGroupFree(&m);
    }
    return;


    while (!candidatesEmpty(&init_cands)){
        const pddl_lifted_mgroup_t *cand = candidatesNext(&init_cands);
        //pddlLiftedMGroupPrint(pddl, cand, stderr);
        if (isInitExactlyOne(pddl, cand, &init_cands)
                && !isAnyActionTooHeavy(pddl, cand, &init_cands)){
            fprintf(stderr, "Hit: ");
            pddlLiftedMGroupPrint(pddl, cand, stderr);
            candidatesAdd(cands, cand);
        }
    }
    candidatesFree(&init_cands);
    return;
    exit(-1);

    for (int pred_id = 0; pred_id < pddl->pred.pred_size; ++pred_id){
        const pddl_pred_t *pred = pddl->pred.pred + pred_id;
        if (pddlPredIsStatic(pred) || pred_id == pddl->pred.eq_pred)
            continue;

        pddl_lifted_mgroup_t m;

        pddlLiftedMGroupInitCandFromPred(&m, pred, -1);
        if (m.param.param_size > 0){
            _initialCandidates(pddl, &m, 0, cands);
        }else{
            candidatesAdd(cands, &m);
        }
        pddlLiftedMGroupFree(&m);
    }
}




/*** PUBLIC API: ***/
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

    pddlLiftedMGroupSort(mgroup);
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
    return isInitTooHeavy(cand, pddl);
}

int pddlLiftedMGroupIsActionTooHeavy(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id)
{
    return isActionTooHeavy(cand, pddl, pddl->action.action + action_id, NULL);
}

int pddlLiftedMGroupIsActionBalanced(const pddl_lifted_mgroup_t *cand,
                                     const pddl_t *pddl,
                                     int action_id)
{
    return isActionBalanced(cand, pddl, pddl->action.action + action_id, NULL);
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

void pddlLiftedMGroupsInitCopy(pddl_lifted_mgroups_t *dst,
                               const pddl_lifted_mgroups_t *src)
{
    pddlLiftedMGroupsInit(dst);
    for (int i = 0; i < src->mgroup_size; ++i)
        pddlLiftedMGroupsAdd(dst, src->mgroup + i);
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

    for (int i = 0; i < src->mgroup_size; ++i){
        const pddl_lifted_mgroup_t *mg = src->mgroup + i;
        pddl_obj_id_t arg[mg->param.param_size];

        pddl_cond_const_it_atom_t it;
        const pddl_cond_atom_t *goal;
        PDDL_COND_FOR_EACH_ATOM(pddl->goal, &it, goal){
            ASSERT(!goal->neg);
            const pddl_cond_atom_t *c;
            FOR_EACH_CAND(mg, c){
                if (c->pred != goal->pred)
                    continue;

                if (unifyFact(pddl, goal, NULL, &mg->param, c, arg))
                    pddlLiftedMGroupsAddInst(dst, mg, arg);
            }
        }
    }
    pddlLiftedMGroupsSortAndUniq(dst);
}

int pddlLiftedMGroupsIsGroundedConjTooHeavy(const pddl_lifted_mgroups_t *mgs,
                                            const pddl_t *pddl,
                                            const pddl_cond_arr_t *c,
                                            const pddl_obj_id_t *args)
{
    for (int i = 0; i < mgs->mgroup_size; ++i){
        if (isGroundedCondArrTooHeavy(mgs->mgroup + i, pddl, c, args))
            return 1;
    }
    return 0;
}

static int mgroupIsDeleted(const pddl_lifted_mgroup_t *mg,
                           const pddl_t *pddl,
                           const pddl_cond_arr_t *pre,
                           const pddl_cond_arr_t *add_eff,
                           const pddl_cond_arr_t *del_eff,
                           const pddl_obj_id_t *args)
{
    pddl_obj_id_t mg_arg[mg->param.param_size];

    // First check whether there is a matching add effect. If there is one,
    // then mg cannot be deleted
    // TODO: We should actually be looking if there is a possibility that
    //       the add effect is not there.
    for (int addi = 0; addi < add_eff->size; ++addi){
        const pddl_cond_atom_t *a = PDDL_COND_CAST(add_eff->cond[addi], atom);
        const pddl_cond_atom_t *m;
        FOR_EACH_CAND(mg, m){
            if (m->pred != a->pred)
                continue;
            if (unifyFact(pddl, a, args, &mg->param, m, mg_arg))
                return 0;
        }
    }

    // Then find out if there is a matching delete effect and precondition
    for (int di = 0; di < del_eff->size; ++di){
        const pddl_cond_atom_t *d = PDDL_COND_CAST(del_eff->cond[di], atom);
        const pddl_cond_atom_t *m;
        FOR_EACH_CAND(mg, m){
            if (m->pred != d->pred)
                continue;
            if (unifyFact(pddl, d, args, &mg->param, m, mg_arg)
                    && equalAtomInArr(d, pre, args)){
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
        if (mgroupIsDeleted(mgs->mgroup + i, pddl,
                            pre, add_eff, del_eff, args)){
            return 1;
        }
    }
    return 0;
}

static int proveOrRefineCandidate(const pddl_t *pddl,
                                  const pddl_lifted_mgroup_t *cand,
                                  candidates_t *cands)
{
    /*
    if (isInitTooHeavyForCountedVars(cand, pddl)){
        // Quickly throw away candidates that cannot be mgroups
        // under any circumstances
        return -1;
    }
    */
    if (!isInitExactlyOne(pddl, cand, cands)){
        //fprintf(stderr, "  !isInitExactlyOne()\n");
        return -1;
    }

    for (int ai = 0; ai < pddl->action.action_size; ++ai){
        const pddl_action_t *a = pddl->action.action + ai;
        if (isActionTooHeavy(cand, pddl, a, cands)){
            //fprintf(stderr, "  too-heavy: %s\n", a->name);
            return -1;
        }
    }

    for (int ai = 0; ai < pddl->action.action_size; ++ai){
        const pddl_action_t *a = pddl->action.action + ai;
        if (!isActionBalanced(cand, pddl, a, cands)){
            //fprintf(stderr, "  unbalanced: %s\n", a->name);
            return -1;
        }
    }

    return 0;
}

static int isSingleFact(const pddl_lifted_mgroup_t *cand)
{
    // TODO: Get rid of this test -- it could be useful to have one fact
    //       fam-group...
    if (cand->cond.size != 1)
        return 0;
    return !candHasCountedVar(cand);
}

static void _refineVariableProved(const pddl_t *pddl,
                                  const pddl_lifted_mgroup_t *cand,
                                  int var,
                                  pddl_lifted_mgroups_t *lm,
                                  candidates_t *cands)
{
    for (; var < cand->param.param_size
            && !cand->param.param[var].is_counted_var; ++var);

    if (var == cand->param.param_size){
        if (proveOrRefineCandidate(pddl, cand, NULL) == 0){
            if (!isSingleFact(cand)){
                pddlLiftedMGroupsAdd(lm, cand);
                // TODO: Get rid of this once refinement can create more
                //       counted variables
                /*
                for (int ai = 0; ai < pddl->action.action_size; ++ai){
                    const pddl_action_t *a = pddl->action.action + ai;
                    refineExtendProved(pddl, a, cand, cands);
                }
                */
            }
        }
    }else{
        pddl_lifted_mgroup_t mg;
        pddlLiftedMGroupInitCopy(&mg, cand);
        ASSERT(mg.param.param[var].is_counted_var);
        mg.param.param[var].is_counted_var = 0;
        _refineVariableProved(pddl, &mg, var + 1, lm, cands);
        pddlLiftedMGroupFree(&mg);

        _refineVariableProved(pddl, cand, var + 1, lm, cands);
    }
}

/** TODO **/
static void refineVariableProved(const pddl_t *pddl,
                                 const pddl_lifted_mgroup_t *cand,
                                 pddl_lifted_mgroups_t *lm,
                                 candidates_t *cands)
{
    if (cand->param.param_size == 0 || !candHasCountedVar(cand))
        return;
    _refineVariableProved(pddl, cand, 0, lm, cands);
}

void pddlLiftedMGroupsInfer(const pddl_t *pddl,
                            const pddl_lifted_mgroups_infer_config_t *cfg,
                            pddl_lifted_mgroups_t *lm)
{
    candidates_t cands;
    candidatesInit(&cands, pddl, cfg);

    initialCandidates(pddl, cfg, &cands);
    while (!candidatesEmpty(&cands) && lm->mgroup_size < cfg->max_mgroups){
        const pddl_lifted_mgroup_t *cand = candidatesNext(&cands);
        //fprintf(stderr, "Cand: ");
        //pddlLiftedMGroupPrint(pddl, cand, stderr);
        if (proveOrRefineCandidate(pddl, cand, &cands) != 0)
            continue;
        //fprintf(stderr, "PROVED\n");

        //if (!isSingleFact(cand)){
            if (isInitTooHeavy(cand, pddl)){
                //fprintf(stderr, "XXX: init-too-heavy\n");
                // TODO
                removeHeavinessByInst(pddl, &pddl->init->cls, cand, lm);
            }else{
                if (!isSingleFact(cand))
                    pddlLiftedMGroupsAdd(lm, cand);
                refineVariableProved(pddl, cand, lm, &cands);
                for (int ai = 0; ai < pddl->action.action_size; ++ai){
                    const pddl_action_t *a = pddl->action.action + ai;
                    refineExtendProved(pddl, a, cand, &cands);
                }
            }
        //}
    }

    pddlLiftedMGroupsSortAndUniq(lm);
    candidatesFree(&cands);
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
