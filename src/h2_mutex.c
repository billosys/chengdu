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
#include "pddl/strips.h"

#define FACT(h2, x, y) ((h2)->fact[(x) * (h2)->fact_size + (y)])

struct h2 {
    int *fact;
    int fact_size;
    int *op_applied;
};
typedef struct h2 h2_t;

static void h2Init(h2_t *h2, const pddl_strips_t *strips)
{
    bzero(h2, sizeof(*h2));
    h2->fact_size = strips->fact.fact_size;
    h2->fact = BOR_CALLOC_ARR(int, h2->fact_size * h2->fact_size);
    h2->op_applied = BOR_CALLOC_ARR(int, strips->op.op_size);
}

static void h2Free(h2_t *h2)
{
    if (h2->fact != NULL)
        BOR_FREE(h2->fact);
    if (h2->op_applied != NULL)
        BOR_FREE(h2->op_applied);
}

/** Returns true if operator is applicable with the currently reachable facts */
static int isApplicable(const pddl_strips_op_t *op, h2_t *h2)
{
    int f1, f2;

    if (h2->op_applied[op->id])
        return 1;

    BOR_ISET_FOR_EACH(&op->pre, f1){
        BOR_ISET_FOR_EACH(&op->pre, f2){
            if (!FACT(h2, f1, f2))
                return 0;
        }
    }

    return 1;
}

/** Returns true if operator is applicable with the additional fact_id */
static int isApplicable2(const pddl_strips_op_t *op, int fact_id, h2_t *h2)
{
    int f1;

    if (!h2->op_applied[op->id])
        return 0;

    BOR_ISET_FOR_EACH(&op->pre, f1){
        if (!FACT(h2, f1, fact_id))
            return 0;
    }

    return 1;
}

/** Apply operator if currently applicable */
static int applyOp(const pddl_strips_op_t *op, h2_t *h2)
{
    int f1, f2;
    int updated = 0;

    if (!isApplicable(op, h2))
        return 0;

    BOR_ISET_FOR_EACH(&op->add_eff, f1){
        if (!h2->op_applied[op->id]){
            // This needs to be run only the first time the operator is
            // applied.
            BOR_ISET_FOR_EACH(&op->add_eff, f2){
                if (!FACT(h2, f1, f2)){
                    FACT(h2, f1, f2) = FACT(h2, f2, f1) = 1;
                    updated = 1;
                }
            }
        }

        for (int fact_id = 0; fact_id < h2->fact_size; ++fact_id){
            if (!FACT(h2, fact_id, fact_id) || FACT(h2, f1, fact_id))
                continue;
            if (borISetHas(&op->add_eff, fact_id)
                    || borISetHas(&op->del_eff, fact_id))
                continue;
            if (isApplicable2(op, fact_id, h2)){
                FACT(h2, f1, fact_id) = FACT(h2, fact_id, f1) = 1;
                updated = 1;
            }
        }
    }

    h2->op_applied[op->id] = 1;
    return updated;
}

void pddlStripsH2Mutex(const pddl_strips_t *strips,
                       //pddl_fact_id_pset_t *mgroups,
                       bor_iset_t *unreachable_facts)
                       // TODO: unreachable ops
{
    h2_t h2;
    int updated;
    const pddl_strips_op_t *op;
    bor_iset_t mgroup;

    h2Init(&h2, strips);
    borISetInit(&mgroup);

    do {
        updated = 0;
        PDDL_STRIPS_OPS_FOR_EACH(&strips->op, op){
            updated |= applyOp(op, &h2);
        }
    } while (updated);

    for (int f1 = 0; f1 < h2.fact_size; ++f1){
        for (int f2 = 0; f2 < h2.fact_size; ++f2){
            if (f1 == f2){
                if (!FACT(&h2, f1, f1))
                    borISetAdd(unreachable_facts, f1);
                continue;
            }

            if (!FACT(&h2, f1, f2)){
                if (f1 == f2){
                    borISetAdd(unreachable_facts, f1);
                }else{
                    borISetEmpty(&mgroup);
                    borISetAdd(&mgroup, f1);
                    borISetAdd(&mgroup, f2);
                    // TODO: pddlFactIdPSetAdd(mgroups, &mgroup);
                }
            }
        }
    }

    // TODO: Unreachable operators

    borISetFree(&mgroup);
    h2Free(&h2);
}
