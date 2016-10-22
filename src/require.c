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

#include "pddl/require.h"

#include "err.h"

/**
 * Mapping between keywords and require flags.
 */
struct _require_mask_t {
    int kw;
    unsigned mask;
};
typedef struct _require_mask_t require_mask_t;

static require_mask_t require_mask[] = {
    { PDDL_KW_STRIPS, PDDL_REQUIRE_STRIPS },
    { PDDL_KW_TYPING, PDDL_REQUIRE_TYPING },
    { PDDL_KW_NEGATIVE_PRE, PDDL_REQUIRE_NEGATIVE_PRE },
    { PDDL_KW_DISJUNCTIVE_PRE, PDDL_REQUIRE_DISJUNCTIVE_PRE },
    { PDDL_KW_EQUALITY, PDDL_REQUIRE_EQUALITY },
    { PDDL_KW_EXISTENTIAL_PRE, PDDL_REQUIRE_EXISTENTIAL_PRE },
    { PDDL_KW_UNIVERSAL_PRE, PDDL_REQUIRE_UNIVERSAL_PRE },
    { PDDL_KW_CONDITIONAL_EFF, PDDL_REQUIRE_CONDITIONAL_EFF },
    { PDDL_KW_NUMERIC_FLUENT, PDDL_REQUIRE_NUMERIC_FLUENT },
    { PDDL_KW_OBJECT_FLUENT, PDDL_REQUIRE_OBJECT_FLUENT },
    { PDDL_KW_DURATIVE_ACTION, PDDL_REQUIRE_DURATIVE_ACTION },
    { PDDL_KW_DURATION_INEQUALITY, PDDL_REQUIRE_DURATION_INEQUALITY },
    { PDDL_KW_CONTINUOUS_EFF, PDDL_REQUIRE_CONTINUOUS_EFF },
    { PDDL_KW_DERIVED_PRED, PDDL_REQUIRE_DERIVED_PRED },
    { PDDL_KW_TIMED_INITIAL_LITERAL, PDDL_REQUIRE_TIMED_INITIAL_LITERAL },
    { PDDL_KW_DURATIVE_ACTION, PDDL_REQUIRE_DURATIVE_ACTION },
    { PDDL_KW_PREFERENCE, PDDL_REQUIRE_PREFERENCE },
    { PDDL_KW_CONSTRAINT, PDDL_REQUIRE_CONSTRAINT },
    { PDDL_KW_ACTION_COST, PDDL_REQUIRE_ACTION_COST },
    { PDDL_KW_MULTI_AGENT, PDDL_REQUIRE_MULTI_AGENT },
    { PDDL_KW_UNFACTORED_PRIVACY, PDDL_REQUIRE_UNFACTORED_PRIVACY },
    { PDDL_KW_FACTORED_PRIVACY, PDDL_REQUIRE_FACTORED_PRIVACY },

    { PDDL_KW_QUANTIFIED_PRE, PDDL_REQUIRE_EXISTENTIAL_PRE |
                                   PDDL_REQUIRE_UNIVERSAL_PRE },
    { PDDL_KW_FLUENTS, PDDL_REQUIRE_NUMERIC_FLUENT |
                            PDDL_REQUIRE_OBJECT_FLUENT },
    { PDDL_KW_ADL, PDDL_REQUIRE_STRIPS |
                        PDDL_REQUIRE_TYPING |
                        PDDL_REQUIRE_NEGATIVE_PRE |
                        PDDL_REQUIRE_DISJUNCTIVE_PRE |
                        PDDL_REQUIRE_EQUALITY |
                        PDDL_REQUIRE_EXISTENTIAL_PRE |
                        PDDL_REQUIRE_UNIVERSAL_PRE |
                        PDDL_REQUIRE_CONDITIONAL_EFF },
};
static int require_mask_size = sizeof(require_mask) / sizeof(require_mask_t);

static unsigned requireMask(int kw)
{
    int i;

    for (i = 0; i < require_mask_size; ++i){
        if (require_mask[i].kw == kw)
            return require_mask[i].mask;
    }
    return 0u;
}

int pddlRequireParse(const pddl_lisp_t *domain, unsigned *req)
{
    const pddl_lisp_node_t *req_node, *n;
    unsigned m;
    int i;

    *req = 0u;
    req_node = pddlLispFindNode(&domain->root, PDDL_KW_REQUIREMENTS);
    // No :requirements implies :strips
    if (req_node == NULL){
        *req = PDDL_REQUIRE_STRIPS;
        return 0;
    }

    for (i = 1; i < req_node->child_size; ++i){
        n = req_node->child + i;
        if (n->value == NULL){
            ERRN2(n, "Invalid :requirements definition.");
            return -1;
        }
        if ((m = requireMask(n->kw)) == 0u){
            ERRN(n, "Invalid :requirements definition: Unkown keyword `%s'.",
                 n->value);
            return -1;
        }

        *req |= m;
    }

    return 0;
}
