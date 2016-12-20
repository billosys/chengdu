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

#ifndef __PDDL_H__
#define __PDDL_H__

#include <pddl/lisp.h>
#include <pddl/require.h>
#include <pddl/type.h>
#include <pddl/obj.h>
#include <pddl/pred.h>
#include <pddl/fact.h>
#include <pddl/action.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Flags: **/
#define PDDL_FORCE_ADL 0x1u /*!< Force ADL to requirements */

struct pddl {
    unsigned flags;
    pddl_lisp_t *domain_lisp;
    pddl_lisp_t *problem_lisp;
    const char *domain_name;
    const char *problem_name;
    unsigned require;
    pddl_types_t type;
    pddl_objs_t obj;
    pddl_preds_t pred;
    pddl_preds_t func;
    pddl_facts_t init_fact;
    pddl_facts_t init_func;
    pddl_facts_t goal;
    pddl_actions_t action;
    int metric;
};
typedef struct pddl pddl_t;

pddl_t *pddlNew(const char *domain_fn, const char *problem_fn,
                unsigned flags);
void pddlDel(pddl_t *pddl);
void pddlDump(const pddl_t *pddl, FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_H__ */
