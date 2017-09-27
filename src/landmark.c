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
#include "pddl/landmark.h"

void pddlDisjunctiveLandmarksInit(pddl_disjunctive_landmarks_t *ldms)
{
    bzero(ldms, sizeof(*ldms));
}

void pddlDisjunctiveLandmarksFree(pddl_disjunctive_landmarks_t *ldms)
{
    for (int i = 0; i < ldms->ldm_size; ++i)
        borISetFree(ldms->ldm + i);
    if (ldms->ldm != NULL)
        BOR_FREE(ldms->ldm);
}

void pddlDisjunctiveLandmarksAdd(pddl_disjunctive_landmarks_t *ldms,
                                 const bor_iset_t *ldm)
{
    if (ldms->ldm_size >= ldms->ldm_alloc){
        if (ldms->ldm_alloc == 0)
            ldms->ldm_alloc = 1;
        ldms->ldm_alloc *= 2;
        ldms->ldm = BOR_REALLOC_ARR(ldms->ldm, bor_iset_t, ldms->ldm_alloc);
    }

    borISetInit(&ldms->ldm[ldms->ldm_size]);
    borISetUnion(&ldms->ldm[ldms->ldm_size], ldm);
    ++ldms->ldm_size;
}
