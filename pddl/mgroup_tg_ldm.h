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

#ifndef __PDDL_MGROUP_TG_LDM_H__
#define __PDDL_MGROUP_TG_LDM_H__

#include <boruvka/iset.h>
#include <boruvka/iarr.h>

#include <pddl/strips.h>
#include <pddl/landmark.h>
#include <pddl/mgroup.h>
#include <pddl/g.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_mgroup_tg_ldm_tg {
    pddl_g_t tg; /*!< Transition graph */
    bor_iset_t mgroup; /*!< mgroups the transition graph is composed of */
    bor_iset_t tg_op; /*!< Operators contained in .tg */
    bor_iset_t tg_fact; /*!< Facts contained in .tg */
    bor_iset_t init_node; /*!< Init nodes of .tg */
    bor_iset_t goal_node; /*!< Goal nodes of .tg */
    bor_iarr_t core_ldm_seq; /*!< Sequence of landmarks from init to goal */
};
typedef struct pddl_mgroup_tg_ldm_tg pddl_mgroup_tg_ldm_tg_t;

struct pddl_mgroup_tg_ldm {
    const pddl_strips_t *strips;
    pddl_landmarks_t ldm; /*!< Pool of landmarks */
    pddl_g_t *mgroup_tg;
    pddl_mgroup_tg_ldm_tg_t **tg;
    int tg_alloc;
    int tg_size;

    bor_iset_t op; /*!< List of operators that ought to remain */
};
typedef struct pddl_mgroup_tg_ldm pddl_mgroup_tg_ldm_t;

void pddlMGroupTGLdmInit(pddl_mgroup_tg_ldm_t *, const pddl_strips_t *);
void pddlMGroupTGLdmFree(pddl_mgroup_tg_ldm_t *);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_MGROUP_TG_LDM_H__ */
