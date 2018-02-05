/***
 * cpddl
 * -------
 * Copyright (c)2018 Daniel Fiser <danfis@danfis.cz>,
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
#include "pddl/sync_product_prep.h"
#include "pddl/sync_product.h"


void pddlSyncProductPrepInit(pddl_sync_product_prep_t *p,
                             int mgroup_id,
                             const pddl_strips_t *strips)
{
    const pddl_mgroup_t *mgroup = strips->mgroup.mgroup + mgroup_id;
    int mgroup_size = borISetSize(&mgroup->fact);
    int i, fact;

    bzero(p, sizeof(*p));
    borISetInit(&p->mgroup);
    borISetAdd(&p->mgroup, mgroup_id);

    p->node_alloc = 2;
    while (p->node_alloc < mgroup_size)
        p->node_alloc *= 2;
    p->node_size = mgroup_size;
    p->node = BOR_CALLOC_ARR(bor_iset_t, p->node_alloc);

    i = 0;
    BOR_ISET_FOR_EACH(&mgroup->fact, fact)
        borISetAdd(&p->node[i++], fact);
}

void pddlSyncProductPrepFree(pddl_sync_product_prep_t *p)
{
    borISetFree(&p->mgroup);

    for (int i = 0; i < p->node_alloc; ++i)
        borISetFree(&p->node[i]);
    if (p->node != NULL)
        BOR_FREE(p->node);

    for (int i = 0; i < p->next_alloc; ++i)
        borISetFree(&p->next[i]);
    if (p->next != NULL)
        BOR_FREE(p->next);
}

static void addNode(pddl_sync_product_prep_t *p,
                    const bor_iset_t *facts)
{
    if (p->next_size == p->next_alloc){
        int alloc = p->next_alloc;
        if (p->next_alloc == 0)
            p->next_alloc = 2;
        p->next_alloc *= 2;
        p->next = BOR_REALLOC_ARR(p->next, bor_iset_t, p->next_alloc);
        for (int i = alloc; i < p->next_alloc; ++i)
            borISetInit(p->next + i);
    }

    borISetSet(p->next + p->next_size, facts);
    ++p->next_size;
}

static void extendNode(pddl_sync_product_prep_t *p,
                       const pddl_strips_t *strips,
                       const bor_iset_t *node,
                       const bor_iset_t *facts)
{
    BOR_ISET(next);
    int fact;

    if (borISetIsDisjunct(node, facts)){
        borISetSet(&next, node);
        BOR_ISET_FOR_EACH(facts, fact){
            borISetAdd(&next, fact);
            if (!pddlMutexesIsMutex(&strips->mutex, &next))
                addNode(p, &next);
            borISetRm(&next, fact);
        }
    }else{
        addNode(p, node);
    }

    borISetFree(&next);
}

static void swap(pddl_sync_product_prep_t *p)
{
    bor_iset_t *tmp_node;
    int tmp;

    tmp_node = p->node;
    p->node = p->next;
    p->next = tmp_node;

    tmp = p->node_alloc;
    p->node_alloc = p->next_alloc;
    p->next_alloc = tmp;
    p->node_size = p->next_size;
    p->next_size = 0;
}

int pddlSyncProductPrepExtend(pddl_sync_product_prep_t *p,
                              int mgroup_id,
                              const pddl_strips_t *strips,
                              size_t max_mem)
{
    const pddl_mgroup_t *mgroup = strips->mgroup.mgroup + mgroup_id;
    int max_nodes = sqrt(max_mem);

    p->next_size = 0;
    for (int i = 0; i < p->node_size; ++i){
        if (p->next_size > max_nodes)
            return -1;
        extendNode(p, strips, p->node + i, &mgroup->fact);
    }

    if (!pddlSyncProductCanFitInMem2(p->next_size,
                                     borISetSize(&p->mgroup) + 1, max_mem)){
        return -1;
    }

    swap(p);
    borISetAdd(&p->mgroup, mgroup_id);
    return 0;
}
