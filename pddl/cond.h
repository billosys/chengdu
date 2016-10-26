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


/**
 * Types of conditions
 */
#define PDDL_COND_AND  1u /*!< Conjuction */
#define PDDL_COND_OR  2u /*!< Disjunction */
#define PDDL_COND_FORALL 4u /*!< Universal quantifier */
#define PDDL_COND_EXIST 5u /*!< Existential quantifier */
#define PDDL_COND_WHEN 6u
#define PDDL_COND_ATOM 7u
#define PDDL_COND_ASSIGN 8u

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


void pddlCondDel(pddl_cond_t *cond);

pddl_cond_t *pddlCondParse(const pddl_lisp_node_t *root,
                           const pddl_types_t *types,
                           const pddl_objs_t *objs,
                           const pddl_type_obj_t *type_obj,
                           const pddl_preds_t *preds,
                           const pddl_preds_t *funcs,
                           const pddl_params_t *params,
                           const char *errname);

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
