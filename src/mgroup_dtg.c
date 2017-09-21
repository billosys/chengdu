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

#include "pddl/pddl.h"
#include "pddl/mgroup_dtg.h"
#include "err.h"

int pddlMGroupDTGInit(pddl_mgroup_dtg_t *dtg,
                      const pddl_mgroup_t *mg,
                      const pddl_strips_t *strips)
{
    int *fact_to_node;
    bor_iset_t pre, add;
    int idx;

    if (strips->has_cond_eff)
        ERR_RET2(-1, "Conditional effects are not supported for mutex"
                     " group DTGs.");

    dtg->node_size = mg->fact.size + 1;
    dtg->adj = BOR_CALLOC_ARR(bor_iset_t, dtg->node_size * dtg->node_size);

    dtg->fact = BOR_ALLOC_ARR(int, dtg->node_size);
    for (int i = 0; i < mg->fact.size; ++i)
        dtg->fact[i] = mg->fact.s[i];
    dtg->fact[dtg->node_size - 1] = -1;

    fact_to_node = BOR_ALLOC_ARR(int, strips->fact.fact_size);
    for (int i = 0; i < mg->fact.size; ++i)
        fact_to_node[mg->fact.s[i]] = i;

    borISetInit(&pre);
    borISetInit(&add);
    for (int i = 0; i < strips->op.op_size; ++i){
        const pddl_strips_op_t *op = strips->op.op[i];
        borISetIntersect2(&pre, &mg->fact, &op->pre);
        borISetIntersect(&pre, &op->del_eff);
        if (borISetSize(&pre) > 0){
            borISetIntersect2(&add, &mg->fact, &op->add_eff);
            idx = fact_to_node[pre.s[0]] * dtg->node_size;
            if (borISetSize(&add) > 0){
                idx += fact_to_node[add.s[0]];
            }else{
                idx += dtg->node_size - 1;
            }
            borISetAdd(&dtg->adj[idx], i);
        }
    }

    borISetFree(&pre);
    borISetFree(&add);
    BOR_FREE(fact_to_node);

    return 0;
}

void pddlMGroupDTGFree(pddl_mgroup_dtg_t *dtg)
{
    int size = dtg->node_size * dtg->node_size;
    for (int i = 0; i < size; ++i)
        borISetFree(&dtg->adj[i]);
    if (dtg->adj != NULL)
        BOR_FREE(dtg->adj);
    if (dtg->fact != NULL)
        BOR_FREE(dtg->fact);
}

void pddlMGroupDTGPrintAsDot(const pddl_mgroup_dtg_t *dtg,
                             const pddl_t *pddl,
                             FILE *fout)
{
    fprintf(fout, "digraph {\n");
    PDDL_MGROUP_DTG_FOR_EACH_NODE(dtg, n){
        fprintf(fout, "N%d [", n);
        if (dtg->fact[n] >= 0){
            const pddl_fact_t *f = pddl->strips->fact.fact[dtg->fact[n]];
            fprintf(fout, "label=\"%s\"", f->name);
            if (borISetIn(dtg->fact[n], &pddl->strips->init))
                fprintf(fout, ", color=\"#bb2222\"");
            if (borISetIn(dtg->fact[n], &pddl->strips->goal))
                fprintf(fout, ", color=\"#2222bb\"");
        }else{
            fprintf(fout, "label=\"none-of-those\"");
        }
        fprintf(fout, "];\n");
    }

    PDDL_MGROUP_DTG_FOR_EACH_NODE(dtg, from){
        bor_iset_t *edge;
        PDDL_MGROUP_DTG_FOR_EACH_EDGE(dtg, from, to, edge){
            fprintf(fout, "N%d -> N%d [", from, to);
            fprintf(fout, "label=\"%d\"", borISetSize(edge));
            fprintf(fout, "];\n");
        }
    }
    fprintf(fout, "}\n");

    printf("====\n");
    PDDL_MGROUP_DTG_FOR_EACH_NODE(dtg, from){
        bor_iset_t *edge;
        int a = 0;
        PDDL_MGROUP_DTG_FOR_EACH_EDGE(dtg, from, to, edge){
            if (a == 0)
                printf("%d ->", from);
            a = 1;
            printf(" %d", to);
        }
        if (a)
            printf("\n");
    }

    printf("---\n");
    PDDL_MGROUP_DTG_FOR_EACH_NODE(dtg, from){
        bor_iset_t *edge;
        int a = 0;
        PDDL_MGROUP_DTG_FOR_EACH_EDGE_REV(dtg, from, to, edge){
            if (a == 0)
                printf("%d ->", from);
            a = 1;
            printf(" %d", to);
        }
        if (a)
            printf("\n");
    }
}
