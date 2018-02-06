/***
 * cpddl
 * -------
 * Copyright (c)2018 Daniel Fiser <danfis@danfis.cz>,
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

#ifndef __PDDL_HEUR_MGROUP_MERGE_H__
#define __PDDL_HEUR_MGROUP_MERGE_H__

#include <boruvka/iset.h>

#include <pddl/common.h>
#include <pddl/strips.h>
#include <pddl/strips_cross_ref.h>
#include <pddl/landmark.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_heur_mgroup_merge_value {
    bor_iset_t fact;
    int value;
};
typedef struct pddl_heur_mgroup_merge_value pddl_heur_mgroup_merge_value_t;

struct pddl_heur_mgroup_merge_merge {
    bor_iset_t mgroup;
    int max_nodes;
    pddl_heur_mgroup_merge_value_t *value;
    int value_size;
    int value_alloc;
    pddl_landmark_seq_t ldm_seq;
};
typedef struct pddl_heur_mgroup_merge_merge pddl_heur_mgroup_merge_merge_t;

struct pddl_heur_mgroup_merge {
    pddl_strips_t *strips;
    pddl_heur_mgroup_merge_merge_t *merge;
    int merge_size;
    int op_cost_scale;
    size_t max_mem;
};
typedef struct pddl_heur_mgroup_merge pddl_heur_mgroup_merge_t;

void pddlHeurMGroupMergeInit(pddl_heur_mgroup_merge_t *h,
                             const pddl_strips_t *strips,
                             int use_cost_part,
                             size_t max_mem);
void pddlHeurMGroupMergeFree(pddl_heur_mgroup_merge_t *h);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_HEUR_MGROUP_MERGE_H__ */
