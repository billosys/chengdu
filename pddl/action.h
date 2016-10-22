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

#ifndef __PDDL_ACTION_H__
#define __PDDL_ACTION_H__

#include <pddl/lisp.h>
#include <pddl/obj.h>
#include <pddl/fact.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_cond_eff {
    pddl_facts_t pre;
    pddl_facts_t eff;
};
typedef struct pddl_cond_eff pddl_cond_eff_t;

struct pddl_cond_effs {
    pddl_cond_eff_t *cond_eff;
    int size;
};
typedef struct pddl_cond_effs pddl_cond_effs_t;

struct pddl_action {
    const char *name;
    pddl_objs_t param;
    pddl_facts_t pre;
    pddl_facts_t eff;
    pddl_facts_t cost;
    pddl_cond_effs_t cond_eff;
};
typedef struct pddl_action pddl_action_t;

struct pddl_actions {
    pddl_action_t *action;
    int size;
};
typedef struct pddl_actions pddl_actions_t;

/**
 * Parses actions from domain PDDL.
 */
int pddlActionsParse(const pddl_lisp_t *domain,
                     const pddl_types_t *types,
                     const pddl_objs_t *objs,
                     const pddl_type_obj_t *type_obj,
                     const pddl_predicates_t *predicates,
                     const pddl_predicates_t *functions,
                     unsigned require,
                     pddl_actions_t *actions);

void pddlActionsFree(pddl_actions_t *actions);

pddl_action_t *pddlActionsAdd(pddl_actions_t *as);

void pddlActionFactPrint(const pddl_predicates_t *predicates,
                         const pddl_objs_t *objs,
                         const pddl_action_t *a,
                         const pddl_fact_t *f,
                         FILE *fout);

void pddlActionsPrint(const pddl_actions_t *actions,
                      const pddl_objs_t *objs,
                      const pddl_predicates_t *predicates,
                      const pddl_predicates_t *functions,
                      FILE *fout);

void pddlCondEffFree(pddl_cond_eff_t *ce);
void pddlCondEffsFree(pddl_cond_effs_t *ce);
void pddlCondEffCopy(pddl_cond_eff_t *dst, const pddl_cond_eff_t *src);
void pddlCondEffsCopy(pddl_cond_effs_t *dst, const pddl_cond_effs_t *src);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_ACTION_H__ */
