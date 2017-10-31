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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "pddl/strips_plan.h"
#include "err.h"

static void planInit(pddl_strips_plan_t *plan)
{
    borIArrInit(&plan->plan);
    borISetInit(&plan->plan_set);
    plan->cost = 0;
}

static int findOp(const pddl_strips_t *strips, const char *op_name)
{
    for (int i = 0; i < strips->op.op_size; ++i){
        if (strcmp(strips->op.op[i]->name, op_name) == 0)
            return i;
    }
    return -1;
}

#define IS_WS(D) \
    ((D) == ' ' || (D) == '\t' || (D) == '\n')
#define SKIP_WS(DATA, IT, SIZE) \
    for (; (IT) < (SIZE) && IS_WS((DATA)[(IT)]); ++(IT))

int pddlStripsPlanLoadFromFile(pddl_strips_plan_t *plan,
                                const pddl_strips_t *strips,
                                const char *fn)
{
    int fd, i;
    struct stat st;
    char *data;
    const char *op_name;
    int op_id;

    planInit(plan);

    fd = open(fn, O_RDONLY);
    if (fd == -1)
        ERR_RET(-1, "Could not not open file `%s'.", fn);

    if (fstat(fd, &st) != 0){
        ERR("Could not determine size of the file `%s'.", fn);
        close(fd);
        return -1;
    }

    data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED){
        ERR("Could not mmap file `%s'.", fn);
        close(fd);
        return -1;
    }

    for (i = 0; i < st.st_size; ++i){
        SKIP_WS(data, i, st.st_size);
        if (data[i] == ';'){
            for (; i < st.st_size && data[i] != '\n'; ++i);

        }else if (data[i] == '('){
            ++i;
            SKIP_WS(data, i, st.st_size);
            if (i >= st.st_size)
                ERR_RET2(-1, "Invalid plan file format: Could not find end"
                             " of the operator name.");

            op_name = data + i;
            for (; i < st.st_size && data[i] != ')'; ++i);
            if (data[i] != ')')
                ERR_RET2(-1, "Invalid plan file format: Could not find end"
                             " of the operator name.");

            for (int j = i; j >= 0 && (data[j] == ')' || IS_WS(data[j])); --j)
                data[j] = 0x0;
            if ((op_id = findOp(strips, op_name)) < 0)
                ERR_RET(-1, "Could not find operator `%s'", op_name);

            borIArrAdd(&plan->plan, op_id);
            borISetAdd(&plan->plan_set, op_id);
            plan->cost += strips->op.op[op_id]->cost;
        }
    }

    munmap((void *)data, st.st_size);
    return 0;
}

void pddlStripsPlanFree(pddl_strips_plan_t *plan)
{
    borIArrFree(&plan->plan);
    borISetFree(&plan->plan_set);
}
