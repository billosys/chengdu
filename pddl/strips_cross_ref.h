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

#ifndef __PDDL_STRIPS_CROSS_REF_H__
#define __PDDL_STRIPS_CROSS_REF_H__

#include <pddl/strips.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_strips_cross_ref_fact {
    bor_iset_t op_pre;
    bor_iset_t op_del;
    bor_iset_t op_add;
};
typedef struct pddl_strips_cross_ref_fact pddl_strips_cross_ref_fact_t;

struct pddl_strips_cross_ref_mgroup {
    bor_iset_t op_pre;
    bor_iset_t op_del;
    bor_iset_t op_add;

    bor_iset_t op; /*!< op_pre \cup op_del \cup op_add */
    bor_iset_t op_del_add; /*!< op_del \cup op_add */
};
typedef struct pddl_strips_cross_ref_mgroup pddl_strips_cross_ref_mgroup_t;

struct pddl_strips_cross_ref {
    const pddl_strips_t *strips;
    pddl_strips_cross_ref_fact_t *fact;
    int fact_size;
    pddl_strips_cross_ref_mgroup_t *mgroup;
    int mgroup_size;
};
typedef struct pddl_strips_cross_ref pddl_strips_cross_ref_t;


/** The following flags turn off the specified cross reference */
#define PDDL_STRIPS_CROSS_REF_NO_FACT_OP_PRE        0x001u
#define PDDL_STRIPS_CROSS_REF_NO_FACT_OP_DEL        0x002u
#define PDDL_STRIPS_CROSS_REF_NO_FACT_OP_ADD        0x004u
#define PDDL_STRIPS_CROSS_REF_NO_MGROUP_OP_PRE      0x008u
#define PDDL_STRIPS_CROSS_REF_NO_MGROUP_OP_DEL      0x010u
#define PDDL_STRIPS_CROSS_REF_NO_MGROUP_OP_ADD      0x020u
#define PDDL_STRIPS_CROSS_REF_NO_MGROUP_OP          0x040u
#define PDDL_STRIPS_CROSS_REF_NO_MGROUP_OP_DEL_ADD  0x080u


/**
 * Cross reference strips objects.
 * If the flags is set to 0, all cross references are filled in.
 */
void pddlStripsCrossRefInit(pddl_strips_cross_ref_t *cr,
                            const pddl_strips_t *strips,
                            unsigned flags);

/**
 * Frees allocated memory.
 */
void pddlStripsCrossRefFree(pddl_strips_cross_ref_t *cr);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_STRIPS_CROSS_REF_H__ */
