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
#include "pddl/fdr_var.h"

void pddlFDRVarsInit(pddl_fdr_vars_t *vars,
                     const pddl_mgroups_t *mg, unsigned flags)
{
    bzero(vars, sizeof(*vars));
}

void pddlFDRVarsFree(pddl_fdr_vars_t *vars)
{
}

void pddlFDRVarsPrint(const pddl_fdr_vars_t *vars, FILE *fout)
{
    for (int i = 0; i < vars->size; ++i){
        fprintf(fout, "Var %d:\n", i);
    }
}
