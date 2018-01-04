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

static void facts(pddl_strips_cross_ref_t *cr,
                  const pddl_strips_t *strips)
{
    for (int opi = 0; opi < strips->op.op_size; ++opi){
        const pddl_strips_op_t *op = strips->op.op[opi];
        int fact;

        BOR_ISET_FOR_EACH(&op->pre, fact)
            borISetAdd(&cr->fact[fact].op_pre, opi);
        BOR_ISET_FOR_EACH(&op->del_eff, fact)
            borISetAdd(&cr->fact[fact].op_del, opi);
        BOR_ISET_FOR_EACH(&op->add_eff, fact)
            borISetAdd(&cr->fact[fact].op_add, opi);

        for (fact = 0; fact < strips->fact.fact_size; ++fact){
            if (pddlMutexesIsMutexWithFact(&strips->mutex, fact, &op->pre))
                borISetAdd(&cr->fact[fact].op_pre_mutex, opi);
        }
    }
}

static void mgroupNoneOfThose(pddl_strips_cross_ref_t *cr,
                              const pddl_mgroup_t *mgroup,
                              pddl_strips_cross_ref_mgroup_t *mg)
{
    BOR_ISET(ops);
    int op_id;

    borISetMinus2(&ops, &mg->op_del, &mg->op_add);
    BOR_ISET_FOR_EACH(&ops, op_id)
        borISetAdd(&cr->fact[mgroup->none_of_those].op_add, op_id);

    borISetMinus2(&ops, &mg->op_add, &mg->op_del);
    BOR_ISET_FOR_EACH(&ops, op_id)
        borISetAdd(&cr->fact[mgroup->none_of_those].op_del, op_id);

    borISetFree(&ops);
}

static void mgroups(pddl_strips_cross_ref_t *cr,
                    const pddl_strips_t *strips)
{
    for (int mi = 0; mi < strips->mgroup.mgroup_size; ++mi){
        const pddl_mgroup_t *mgroup = strips->mgroup.mgroup + mi;
        pddl_strips_cross_ref_mgroup_t *mg = cr->mgroup + mi;
        int fact;

        BOR_ISET_FOR_EACH(&mgroup->fact, fact){
            borISetUnion(&mg->op_pre, &cr->fact[fact].op_pre);
            borISetUnion(&mg->op_del, &cr->fact[fact].op_del);
            borISetUnion(&mg->op_add, &cr->fact[fact].op_add);
            borISetAdd(&cr->fact[fact].mgroup, mi);
        }

        borISetUnion2(&mg->op, &mg->op_del, &mg->op_add);
        borISetUnion(&mg->op, &mg->op_pre);
        borISetUnion2(&mg->op_del_add, &mg->op_del, &mg->op_add);

        if (mgroup->none_of_those >= 0)
            mgroupNoneOfThose(cr, mgroup, mg);
    }
}

static void opOnlyOnce(const pddl_strips_cross_ref_t *cref,
                       const pddl_strips_t *strips,
                       bor_iset_t *op_only_once)
{
    BOR_ISET(pre_del);
    int fact;

    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        borISetIntersect2(&pre_del, &op->pre, &op->del_eff);
        BOR_ISET_FOR_EACH(&pre_del, fact){
            if (borISetSize(&cref->fact[fact].op_add) == 0)
                borISetAdd(op_only_once, i);
        }
    }

    borISetFree(&pre_del);
}


// TODO: Better flags
void pddlStripsCrossRefInit(pddl_strips_cross_ref_t *cr,
                            const pddl_strips_t *strips)
{
    bzero(cr, sizeof(*cr));
    cr->strips = strips;
    cr->fact_size = strips->fact.fact_size;
    cr->fact = BOR_CALLOC_ARR(pddl_strips_cross_ref_fact_t, cr->fact_size);
    cr->mgroup_size = strips->mgroup.mgroup_size;
    cr->mgroup = BOR_CALLOC_ARR(pddl_strips_cross_ref_mgroup_t,
                                cr->mgroup_size);

    facts(cr, strips);
    mgroups(cr, strips);
    opOnlyOnce(cr, strips, &cr->op_only_once);
}

void pddlStripsCrossRefFree(pddl_strips_cross_ref_t *cr)
{
    for (int i = 0; i < cr->fact_size; ++i){
        borISetFree(&cr->fact[i].op_pre);
        borISetFree(&cr->fact[i].op_pre_mutex);
        borISetFree(&cr->fact[i].op_del);
        borISetFree(&cr->fact[i].op_add);
        borISetFree(&cr->fact[i].mgroup);
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

    borISetFree(&cr->op_only_once);
}
