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

#include <boruvka/alloc.h>

#include "pddl/strips_cross_ref.h"

#define TEST_FLAG(F, T, M) \
    (!((F) & PDDL_STRIPS_CROSS_REF_NO_##T##_##M))

void pddlStripsCrossRefInit(pddl_strips_cross_ref_t *cr,
                            const pddl_strips_t *strips,
                            unsigned flags)
{
    const pddl_strips_op_t *op;
    const pddl_mgroup_t *mgroup;
    pddl_strips_cross_ref_mgroup_t *mg;
    int fact;

    bzero(cr, sizeof(*cr));
    cr->strips = strips;
    cr->fact_size = strips->fact.fact_size;
    cr->fact = BOR_CALLOC_ARR(pddl_strips_cross_ref_fact_t, cr->fact_size);
    cr->mgroup_size = strips->mgroup.mgroup_size;
    cr->mgroup = BOR_CALLOC_ARR(pddl_strips_cross_ref_mgroup_t,
                                cr->mgroup_size);

    for (int opi = 0; opi < strips->op.op_size; ++opi){
        op = strips->op.op[opi];
        if (TEST_FLAG(flags, FACT, OP_PRE)){
            BOR_ISET_FOR_EACH(&op->pre, fact)
                borISetAdd(&cr->fact[fact].op_pre, opi);
        }
        if (TEST_FLAG(flags, FACT, OP_DEL)){
            BOR_ISET_FOR_EACH(&op->del_eff, fact)
                borISetAdd(&cr->fact[fact].op_del, opi);
        }
        if (TEST_FLAG(flags, FACT, OP_ADD)){
            BOR_ISET_FOR_EACH(&op->add_eff, fact)
                borISetAdd(&cr->fact[fact].op_add, opi);
        }
    }

    for (int mi = 0; mi < strips->mgroup.mgroup_size; ++mi){
        mgroup = strips->mgroup.mgroup + mi;
        mg = cr->mgroup + mi;
        BOR_ISET_FOR_EACH(&mgroup->fact, fact){
            if (TEST_FLAG(flags, MGROUP, OP_PRE))
                borISetUnion(&mg->op_pre, &cr->fact[fact].op_pre);
            if (TEST_FLAG(flags, MGROUP, OP_DEL))
                borISetUnion(&mg->op_del, &cr->fact[fact].op_del);
            if (TEST_FLAG(flags, MGROUP, OP_ADD))
                borISetUnion(&mg->op_add, &cr->fact[fact].op_add);
        }

        if (TEST_FLAG(flags, MGROUP, OP)){
            borISetUnion2(&mg->op, &mg->op_del, &mg->op_add);
            borISetUnion(&mg->op, &mg->op_pre);
        }
        if (TEST_FLAG(flags, MGROUP, OP_DEL_ADD))
            borISetUnion2(&mg->op_del_add, &mg->op_del, &mg->op_add);
    }
}

void pddlStripsCrossRefFree(pddl_strips_cross_ref_t *cr)
{
    for (int i = 0; i < cr->fact_size; ++i){
        borISetFree(&cr->fact[i].op_pre);
        borISetFree(&cr->fact[i].op_del);
        borISetFree(&cr->fact[i].op_add);
    }
    if (cr->fact != NULL)
        BOR_FREE(cr->fact);

    for (int i = 0; i < cr->mgroup_size; ++i){
        borISetFree(&cr->mgroup[i].op_pre);
        borISetFree(&cr->mgroup[i].op_del);
        borISetFree(&cr->mgroup[i].op_add);

        borISetFree(&cr->mgroup[i].op);
        borISetFree(&cr->mgroup[i].op_del_add);
    }
    if (cr->mgroup != NULL)
        BOR_FREE(cr->mgroup);
}
