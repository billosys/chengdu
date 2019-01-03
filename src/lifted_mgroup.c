/***
 * cpddl
 * -------
 * Copyright (c)2019 Daniel Fiser <danfis@danfis.cz>,
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

#include "pddl/lifted_mgroup.h"

void pddlLiftedMGroupInitCandFromPred(pddl_lifted_mgroup_t *mgroup,
                                      int pred_id,
                                      int counted_var)
{
}

void pddlLiftedMGroupFree(pddl_lifted_mgroup_t *mgroup)
{
}

int pddlLiftedMGroupIsValid(const pddl_lifted_mgroup_t *cand,
                            const pddl_t *pddl)
{
    return 0;
}

int pddlLiftedMGroupIsActionTooHeavy(const pddl_lifted_mgroup_t *mgroup,
                                     const pddl_t *pddl,
                                     int action_id)
{
    return 0;
}

int pddlLiftedMGroupIsActionUnbalanced(const pddl_lifted_mgroup_t *mgroup,
                                       const pddl_t *pddl,
                                       int action_id)
{
    return 0;
}


void pddlLiftedMGroupPrint(const pddl_t *pddl,
                           const pddl_lifted_mgroup_t *mgroup,
                           FILE *fout)
{
}
