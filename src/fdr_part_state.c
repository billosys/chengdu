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

#include "pddl/fdr_part_state.h"
#include "err.h"

void pddlFDRPartStateInit(pddl_fdr_part_state_t *s,
                          const pddl_fdr_vars_t *vars)
{
    bzero(s, sizeof(*s));
    s->size = vars->size;
    s->val = BOR_ALLOC_ARR(int, s->size);
    for (int i = 0; i < s->size; ++i)
        s->val[i] = PDDL_FDR_VAL_UNDEF;
}

void pddlFDRPartStateFree(pddl_fdr_part_state_t *s)
{
    if (s->val != NULL)
        BOR_FREE(s->val);
}

int pddlFDRPartStateSet(pddl_fdr_part_state_t *s, int var, int val)
{
    if (s->val[var] != PDDL_FDR_VAL_UNDEF)
        ERR_RET(-1, "The variable (%d) is already set.", var);
    s->val[var] = val;
    return 0;
}

int pddlFDRPartStateIsSet(const pddl_fdr_part_state_t *s, int var)
{
    return s->val[var] != PDDL_FDR_VAL_UNDEF;
}

void pddlFDRPartStatePrettyPrint(const pddl_fdr_part_state_t *ps,
                                 const pddl_fdr_vars_t *vars,
                                 FILE *fout)
{
    int first = 1;
    for (int i = 0; i < vars->size; ++i){
        if (pddlFDRPartStateIsSet(ps, i))
            fprintf(fout, " %d:%d", i, ps->val[i]);
    }

    fprintf(fout, " |");
    for (int i = 0; i < vars->size; ++i){
        if (pddlFDRPartStateIsSet(ps, i)){
            if (!first)
                fprintf(fout, ",");
            fprintf(fout, " %d:%s", i, vars->var[i].val[ps->val[i]].name);
            first = 0;
        }
    }
    fprintf(fout, "\n");
}
