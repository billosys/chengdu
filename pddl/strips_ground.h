/***
 * cpddl
 * -------
 * Copyright (c)2018 Daniel Fiser <danfis@danfis.cz>,
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

#ifndef __PDDL_STRIPS_GROUND_H__
#define __PDDL_STRIPS_GROUND_H__

#include <boruvka/htable.h>
#include <boruvka/iset.h>

#include <pddl/common.h>
#include <pddl/strips.h>
#include <pddl/ground_atom.h>
#include <pddl/prep_action.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Forward declaration */
typedef struct pddl_strips_ground_tree pddl_strips_ground_tree_t;
typedef struct pddl_strips_ground_args pddl_strips_ground_args_t;

struct pddl_strips_ground_args_arr {
    pddl_strips_ground_args_t *arg;
    int size;
    int alloc;
};
typedef struct pddl_strips_ground_args_arr pddl_strips_ground_args_arr_t;

struct pddl_strips_ground {
    const pddl_t *pddl;
    pddl_ground_config_t cfg;
    bor_err_t *err;
    pddl_prep_actions_t action;

    pddl_ground_atoms_t static_facts;
    int static_facts_unified;
    pddl_ground_atoms_t facts;
    int unify_start_idx;
    int *ground_atom_to_fact_id;
    pddl_ground_atoms_t funcs;
    pddl_strips_ground_tree_t *tree;
    pddl_strips_ground_args_arr_t ground_args;
};
typedef struct pddl_strips_ground pddl_strips_ground_t;

/**
 * Ground PDDL into STRIPS.
 * It runs:
 *  pddlStripsGroundStart()
 *  pddlStripsGroundUnifyStep()
 *  pddlStripsGroundFinalize()
 */
int pddlStripsGround(pddl_strips_t *strips,
                     const pddl_t *pddl,
                     const pddl_ground_config_t *cfg,
                     bor_err_t *err);


int pddlStripsGroundStart(pddl_strips_ground_t *g,
                          const pddl_t *pddl,
                          const pddl_ground_config_t *cfg,
                          bor_err_t *err);
int pddlStripsGroundUnifyStep(pddl_strips_ground_t *g);
int pddlStripsGroundFinalize(pddl_strips_ground_t *g, pddl_strips_t *strips);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_H__ */

