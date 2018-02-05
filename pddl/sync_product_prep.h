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

#ifndef __PDDL_SYNC_PRODUCT_PREP_H__
#define __PDDL_SYNC_PRODUCT_PREP_H__

#include <boruvka/iset.h>
#include <pddl/strips.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct pddl_sync_product_prep {
    bor_iset_t mgroup;

    bor_iset_t *node;
    int node_size;
    int node_alloc;
    bor_iset_t *next;
    int next_size;
    int next_alloc;
};
typedef struct pddl_sync_product_prep pddl_sync_product_prep_t;

void pddlSyncProductPrepInit(pddl_sync_product_prep_t *p,
                             int mgroup_id,
                             const pddl_strips_t *strips);

void pddlSyncProductPrepFree(pddl_sync_product_prep_t *p);

int pddlSyncProductPrepExtend(pddl_sync_product_prep_t *p,
                              int mgroup_id,
                              const pddl_strips_t *strips,
                              size_t max_mem);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_SYNC_PRODUCT_PREP_H__ */
