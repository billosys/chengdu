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

#include <boruvka/alloc.h>
#include "pddl/strips.h"

static char *groundOpName(const pddl_t *pddl,
                          const pddl_action_t *action,
                          const int *args)
{
    int i, slen;
    char *name, *cur;

    slen = strlen(action->name) + 2 + 1;
    for (i = 0; i < action->param.size; ++i)
        slen += 1 + strlen(pddl->obj.obj[args[i]].name);

    cur = name = BOR_ALLOC_ARR(char, slen);
    cur += sprintf(cur, "(%s", action->name);
    for (i = 0; i < action->param.size; ++i)
        cur += sprintf(cur, " %s", pddl->obj.obj[args[i]].name);
    cur += sprintf(cur, ")");

    return name;
}


static void fullGroundOpRec(pddl_strips_t *strips,
                            const pddl_t *pddl,
                            const pddl_action_t *action,
                            int *args, int argi)
{
    const int *objs;
    int size, i;

    if (action->param.size == argi){
        char *name = groundOpName(pddl, action, args);
        fprintf(stdout, "op: %s\n", name);
        BOR_FREE(name);

    }else{
        objs = pddlTypesObjsByType(&pddl->type, action->param.param[argi].type,
                                   &size);
        for (i = 0; i < size; ++i){
            args[argi] = objs[i];
            fullGroundOpRec(strips, pddl, action, args, argi + 1);
        }
    }
}

static void fullGroundOp(pddl_strips_t *strips,
                         const pddl_t *pddl,
                         const pddl_action_t *action,
                         unsigned flags)
{
    int args[action->param.size];
    fullGroundOpRec(strips, pddl, action, args, 0);
}

int pddlStripsFromPDDLFull(pddl_strips_t *strips,
                           const pddl_t *pddl,
                           unsigned flags)
{
    int i;

    for (i = 0; i < pddl->action.size; ++i){
        fullGroundOp(strips, pddl, pddl->action.action + i, flags);
    }

    return 0;
}
