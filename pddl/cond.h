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

#ifndef __PDDL_COND_H__
#define __PDDL_COND_H__

#include <boruvka/list.h>

#include <pddl/lisp.h>
#include <pddl/require.h>
#include <pddl/type.h>
#include <pddl/param.h>
#include <pddl/obj.h>
#include <pddl/pred.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl;

/**
 * Types of conditions
 */
#define PDDL_COND_AND    0u /*!< Conjuction */
#define PDDL_COND_OR     1u /*!< Disjunction */
#define PDDL_COND_FORALL 2u /*!< Universal quantifier */
#define PDDL_COND_EXIST  3u /*!< Existential quantifier */
#define PDDL_COND_WHEN   4u /*!< Conditional effect */
#define PDDL_COND_ATOM   5u
#define PDDL_COND_ASSIGN 6u
#define PDDL_COND_BOOL   7u

#define PDDL_COND_CAST(C, T) \
    (bor_container_of((C), pddl_cond_##T##_t, cls))

/**
 * General condition
 */
struct pddl_cond {
    unsigned type;   /*!< Type of the condition */
    bor_list_t conn; /*!< Connection to the parent cond */
};
typedef struct pddl_cond pddl_cond_t;

/**
 * Conuction / Disjunction
 */
struct pddl_cond_part {
    pddl_cond_t cls;
    bor_list_t part; /*!< List of parts */
};
typedef struct pddl_cond_part pddl_cond_part_t;

/**
 * Quantifiers
 */
struct pddl_cond_quant {
    pddl_cond_t cls;
    pddl_params_t param; /*!< List of parameters */
    pddl_cond_t *cond;   /*!< Quantified condition */
};
typedef struct pddl_cond_quant pddl_cond_quant_t;

/**
 * Conditional effect
 */
struct pddl_cond_when {
    pddl_cond_t cls;
    pddl_cond_t *pre;
    pddl_cond_t *eff;
};
typedef struct pddl_cond_when pddl_cond_when_t;


/**
 * Argument of an atom
 */
struct pddl_cond_atom_arg {
    int param; /*!< -1 or index of parameter */
    int obj;   /*!< -1 or object ID (constant) */
};
typedef struct pddl_cond_atom_arg pddl_cond_atom_arg_t;

/**
 * Atom
 */
struct pddl_cond_atom {
    pddl_cond_t cls;
    int pred;                  /*!< Predicate ID */
    pddl_cond_atom_arg_t *arg; /*!< List of arguments */
    int arg_size;              /*!< Number of arguments */
    int neg;                   /*!< True if negated */
};
typedef struct pddl_cond_atom pddl_cond_atom_t;


/**
 * Assign
 * TODO: For now only (increase (total-cost) (...)) is supported
 */
struct pddl_cond_assign {
    pddl_cond_t cls;
    int value;                /*!< Assigned immediate value */
    pddl_cond_atom_t *fvalue; /*!< Assigned value through function symbol */
};
typedef struct pddl_cond_assign pddl_cond_assign_t;

/**
 * Boolean value
 */
struct pddl_cond_bool {
    pddl_cond_t cls;
    int val;
};
typedef struct pddl_cond_bool pddl_cond_bool_t;


/**
 * Free memory.
 */
void pddlCondDel(pddl_cond_t *cond);

/**
 * Creates an exact copy of the condition.
 */
pddl_cond_t *pddlCondClone(const pddl_cond_t *cond);

/**
 * Traverse all conditionals in a tree and call in pre/post order callbacks
 * if non-NULL.
 * If pre returns -1 the element is skipped (it is not traversed deeper).
 * If pre returns -2 the whole traversing is terminated.
 * If post returns non-zero value the whole traversing is terminated.
 */
void pddlCondTraverse(pddl_cond_t *c,
                     int (*pre)(pddl_cond_t *, void *),
                     int (*post)(pddl_cond_t *, void *),
                     void *u);

/**
 * Same as pddlCondTraverse() but pddl_cond_t structures are passed so that
 * they can be safely changed within callbacks.
 * The return values of pre and post and treated the same way as in
 * pddlCondTraverse().
 */
void pddlCondRebuild(pddl_cond_t **c,
                     int (*pre)(pddl_cond_t **, void *),
                     int (*post)(pddl_cond_t **, void *),
                     void *userdata);

/**
 * Parse condition from PDDL lisp.
 */
pddl_cond_t *pddlCondParse(const pddl_lisp_node_t *root,
                           struct pddl *pddl,
                           const pddl_params_t *params,
                           const char *errname);

/**
 * Creates a placeholder for an empty precondition.
 */
pddl_cond_t *pddlCondEmptyPre(void);

/**
 * Transforms atom into (and atom).
 */
pddl_cond_t *pddlCondAtomToAnd(pddl_cond_t *atom);

/**
 * Adds {c} to and/or condition.
 */
void pddlCondPartAdd(pddl_cond_part_t *part, pddl_cond_t *c);

/**
 * Returns 0 if cond is a correct precondition, -1 otherwise.
 * If verbose is set, error messages are print to stderr.
 */
int pddlCondCheckPre(const pddl_cond_t *cond,
                     int require,
                     int verbose);

/**
 * Same as pddlCondCheckPre() buf effect is checked.
 */
int pddlCondCheckEff(const pddl_cond_t *cond,
                     int require,
                     int verbose);


/**
 * Set .read to true for all found atoms.
 */
void pddlCondSetPredRead(const pddl_cond_t *cond, pddl_preds_t *preds);

/**
 * Set .write to true for all found atoms, and set .read to true for all
 * atoms found as precondtions in (when ) statement.
 */
void pddlCondSetPredReadWriteEff(const pddl_cond_t *cond, pddl_preds_t *preds);

/**
 * Normalize conditionals by instantiation qunatifiers and transformation to
 * DNF so that the actions can be split.
 */
pddl_cond_t *pddlCondNormalize(pddl_cond_t *cond, const pddl_types_t *types);

/**
 * Ground atom to a fact using arguments, {fact} has to have allocated
 * enough space in .arg[].
 */
void pddlCondAtomGroundFact(const pddl_cond_atom_t *atom,
                            const int *args,
                            pddl_fact_t *fact);

/**
 * Traverses all atoms in pre and grounds them into a fact and calls the
 * provided callback on them. If callback returns something different then
 * 0, the grounding is terminated prematurelly and the same value is
 * returned by the function.
 * If function returns -1, something different then (and ) and atom
 * elements was found and grounding was prematurelly terminated.
 * On success, 0 is returned.
 */
int _pddlCondGroundPre(const struct pddl *pddl,
                      const pddl_cond_t *pre,
                      const int *args,
                      int (*cb)(const pddl_cond_atom_t *atom,
                                const pddl_fact_t *fact,
                                void *),
                      void *userdata);

/**
 * Traverses eff and grounds all found atoms into facts.
 * TODO
 * If any callback returns something different then 0, the grounding is
 * terminated prematurelly and the same value is returned by the function.
 * If function returns -1, something different then (and ), atom, (assign
 * ), or (when ) was found and grounding was prematurelly terminated.
 * On success, 0 is returned.
 */
int _pddlCondGroundEff(const struct pddl *pddl,
                      const pddl_cond_t *eff,
                      const int *args,
                      int (*add_eff)(const pddl_cond_atom_t *atom,
                                     const pddl_fact_t *fact,
                                     void *),
                      int (*del_eff)(const pddl_cond_atom_t *atom,
                                     const pddl_fact_t *fact,
                                     void *),
                      int (*assign)(const pddl_cond_assign_t *assign,
                                    int value,
                                    const pddl_fact_t *fvalue,
                                    void *),
                      int (*when)(const pddl_cond_when_t *when,
                                  void *),
                      void *userdata);

/**
 * Ground preconditions to a list of facts.
 */
int pddlCondGroundPre(const pddl_cond_t *c,
                      const int *arg,
                      pddl_facts_t *facts,
                      pddl_fact_id_arr_t *out,
                      int add_fact);

/**
 * Ground preconditions to a list of facts and a cost.
 * // TODO: Conditional effects
 */
int pddlCondGroundEff(const pddl_cond_t *c,
                      const int *arg,
                      pddl_facts_t *facts,
                      pddl_facts_t *funcs,
                      pddl_fact_id_arr_t *add_eff,
                      pddl_fact_id_arr_t *del_eff,
                      int *cost);

void pddlCondPrint(const pddl_cond_t *cond,
                   const pddl_objs_t *objs,
                   const pddl_preds_t *predicates,
                   const pddl_preds_t *functions,
                   const pddl_params_t *params,
                   FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_COND_H__ */
