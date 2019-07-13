/***
 * cpddl
 * -------
 * Copyright (c)2018 Daniel Fiser <danfis@danfis.cz>,
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
#include <pddl/disambiguation.h>
#include "assert.h"

static void selectExactlyOneMGroups(pddl_mgroups_t *mg,
                                    const pddl_mgroups_t *mgroup)
{
    for (int mi = 0; mi < mgroup->mgroup_size; ++mi){
        const pddl_mgroup_t *m = mgroup->mgroup + mi;
        if (m->is_exactly_one)
            pddlMGroupsAdd(mg, &m->mgroup);
    }
}

void pddlDisambiguateInit(pddl_disambiguate_t *dis,
                          int fact_size,
                          const pddl_mutex_pairs_t *mutex,
                          const pddl_mgroups_t *mgroup_in)
{
    pddl_mgroups_t mgroup;

    bzero(dis, sizeof(*dis));

    pddlMGroupsInitEmpty(&mgroup);
    selectExactlyOneMGroups(&mgroup, mgroup_in);
    if (mgroup.mgroup_size == 0){
        pddlMGroupsFree(&mgroup);
        return;
    }

    dis->fact_size = fact_size;
    dis->mgroup_size = mgroup.mgroup_size;

    dis->fact = BOR_CALLOC_ARR(pddl_disambiguate_fact_t, dis->fact_size);
    for (int fact_id = 0; fact_id < fact_size; ++fact_id){
        pddl_disambiguate_fact_t *f = dis->fact + fact_id;
        pddlBitsetInit(&f->mgroup, dis->mgroup_size);
        pddlBitsetInit(&f->fact, dis->fact_size);
    }

    // Set .mgroup to mgroups the fact belongs to (plus  mgroups that are not
    // exactly-one)
    dis->mgroup = BOR_CALLOC_ARR(pddl_disambiguate_mgroup_t, dis->mgroup_size);
    for (int mi = 0; mi < mgroup.mgroup_size; ++mi){
        const pddl_mgroup_t *mg = mgroup.mgroup + mi;
        pddl_disambiguate_mgroup_t *dm = dis->mgroup + mi;
        pddlBitsetInit(&dm->fact, dis->fact_size);

        int fact_id;
        BOR_ISET_FOR_EACH(&mg->mgroup, fact_id){
            pddlBitsetSet(&dm->fact, fact_id);
            pddlBitsetSet(&dis->fact[fact_id].mgroup, mi);
        }
    }

    // Set .fact to facts that are mutex with that fact.
    PDDL_MUTEX_PAIRS_FOR_EACH(mutex, fact1, fact2){
        pddlBitsetSet(&dis->fact[fact1].fact, fact2);
        pddlBitsetSet(&dis->fact[fact2].fact, fact1);
    }

    // Negate .mgroup and .fact bitsets to get what we really want.
    for (int fact_id = 0; fact_id < fact_size; ++fact_id){
        pddlBitsetNeg(&dis->fact[fact_id].mgroup);
        pddlBitsetNeg(&dis->fact[fact_id].fact);
    }

    pddlBitsetInit(&dis->cur_mgroup, dis->mgroup_size);
    pddlBitsetInit(&dis->cur_mgroup_it, dis->mgroup_size);
    pddlBitsetInit(&dis->cur_allowed_facts, dis->fact_size);
    pddlBitsetInit(&dis->cur_allowed_facts_from_mgroup, dis->fact_size);

    pddlMGroupsFree(&mgroup);
}

void pddlDisambiguateFree(pddl_disambiguate_t *dis)
{
    for (int i = 0; i < dis->fact_size; ++i){
        pddlBitsetFree(&dis->fact[i].mgroup);
        pddlBitsetFree(&dis->fact[i].fact);
    }
    if (dis->fact != NULL)
        BOR_FREE(dis->fact);

    for (int i = 0; i < dis->mgroup_size; ++i)
        pddlBitsetFree(&dis->mgroup[i].fact);
    if (dis->mgroup != NULL)
        BOR_FREE(dis->mgroup);

    pddlBitsetFree(&dis->cur_mgroup);
    pddlBitsetFree(&dis->cur_mgroup_it);
    pddlBitsetFree(&dis->cur_allowed_facts);
    pddlBitsetFree(&dis->cur_allowed_facts_from_mgroup);
}

/** Set dis->cur_mgroup to intersection of .mgroup of all facts
 *  and set dis->cur_allowed_facts to intersection of .fact of all facts. */
static void disambInitCur(pddl_disambiguate_t *dis, const bor_iset_t *facts)
{
    int size = borISetSize(facts);
    int fact_id = borISetGet(facts, 0);
    pddlBitsetCopy(&dis->cur_mgroup, &dis->fact[fact_id].mgroup);
    pddlBitsetCopy(&dis->cur_allowed_facts, &dis->fact[fact_id].fact);
    for (int i = 1; i < size; ++i){
        fact_id = borISetGet(facts, i);
        pddlBitsetAnd(&dis->cur_mgroup, &dis->fact[fact_id].mgroup);
        pddlBitsetAnd(&dis->cur_allowed_facts, &dis->fact[fact_id].fact);
    }
}

int pddlDisambiguateSet(pddl_disambiguate_t *dis, bor_iset_t *set)
{
    if (dis->mgroup_size == 0)
        return 0;

    int count, ext;
    int change = 0, local_change;
    int mgroup_id;

    disambInitCur(dis, set);

    do {
        local_change = 0;
        // Start iterating over mgroups that has empty intersection with
        // the disambiguated set.
        pddlBitsetCopy(&dis->cur_mgroup_it, &dis->cur_mgroup);
        pddlBitsetItStart(&dis->cur_mgroup_it);

        while ((mgroup_id = pddlBitsetItNext(&dis->cur_mgroup_it)) >= 0){
            // Find whether there is only one option in the mgroup that can
            // be true in the set.
            count = pddlBitsetAnd2Cnt1(&dis->cur_allowed_facts_from_mgroup,
                                       &dis->mgroup[mgroup_id].fact,
                                       &dis->cur_allowed_facts);
            if (count == 0){
                // If none of the facts from the mgroup can be true, than
                // this set of facts is not reachable
                return -1;

            }else if (count != 1){
                // More than one option so nothing interesting here
                continue;
            }

            // Extract the only option possible and extend the
            // disambiguated set of facts.
            pddlBitsetItStart(&dis->cur_allowed_facts_from_mgroup);
            ext = pddlBitsetItNext(&dis->cur_allowed_facts_from_mgroup);
            ASSERT(ext >= 0);
            borISetAdd(set, ext);
            change = local_change = 1;

            // Update the set of allowed facts and set of disjunct mutex
            // groups.
            pddlBitsetAnd(&dis->cur_allowed_facts, &dis->fact[ext].fact);
            pddlBitsetAnd(&dis->cur_mgroup, &dis->fact[ext].mgroup);
        }
    } while (local_change);

    return change;
}
