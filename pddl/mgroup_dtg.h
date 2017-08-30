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

#ifndef __PDDL_MGROUP_DTG_H__
#define __PDDL_MGROUP_DTG_H__

#include <boruvka/iset.h>
#include <pddl/mgroup.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_t;

struct pddl_mgroup_dtg {
    int node_size;
    bor_iset_t *adj;
    int *fact;
};
typedef struct pddl_mgroup_dtg pddl_mgroup_dtg_t;

#define PDDL_MGROUP_DTG_FOR_EACH_NODE(DTG, NODE) \
    for (int NODE = 0; NODE < (DTG)->node_size; ++NODE)

#define PDDL_MGROUP_DTG_FOR_EACH_EDGE(DTG, FROM, TO, EDGE) \
    for (int __off = (FROM) * (DTG)->node_size, TO = 0; \
            TO < (DTG)->node_size && ((EDGE) = (DTG)->adj + __off + TO); ++TO) \
        if (borISetSize(EDGE) > 0)

#define PDDL_MGROUP_DTG_FOR_EACH_EDGE_REV(DTG, FROM, TO, EDGE) \
    for (int TO = 0; TO < (DTG)->node_size \
            && ((EDGE) = (DTG)->adj + (FROM) + (DTG)->node_size * TO); ++TO) \
        if (borISetSize(EDGE) > 0)


int pddlMGroupDTGInit(pddl_mgroup_dtg_t *dtg,
                      const pddl_mgroup_t *mg,
                      const pddl_strips_t *strips);
void pddlMGroupDTGFree(pddl_mgroup_dtg_t *dtg);

void pddlMGroupDTGPrintAsDot(const pddl_mgroup_dtg_t *dtg,
                             const struct pddl *pddl,
                             FILE *fout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_MGROUP_DTG_H__ */
