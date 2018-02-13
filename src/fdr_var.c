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
#include <boruvka/sort.h>

#include "pddl/strips.h"
#include "pddl/fdr_var.h"
#include "assert.h"

void pddlFDRValFree(pddl_fdr_val_t *val)
{
    if (val->name != NULL)
        BOR_FREE(val->name);
}

void pddlFDRVarInit(pddl_fdr_var_t *var)
{
    bzero(var, sizeof(*var));
}

void pddlFDRVarFree(pddl_fdr_var_t *var)
{
    for (int i = 0; i < var->size; ++i)
        pddlFDRValFree(var->val + i);
    if (var->val != NULL)
        BOR_FREE(var->val);
}

/** If an operator contains a fact in its delete effect such that the fact
 *  is not part of any mutex group that has intersection with its
 *  precondition of add effect, then this fact must be encoded separately
 *  (or this feature must be compiled away).
 *  Consider the following example:
 *      operator: pre: f_1, add: f_2, del: f_3
 *      mutex group: { f_3, f_4 }
 *  Now, how can we encode del: f_3 using the given mutex group?
 *  If we encode it as "none-of-those", then the resulting state from the
 *  application of the operator on the state {f_1, f_4} would be incorrect.
 *  This is why f_3 must be encoded separately from all other mutex groups.
 */
static void factsRequiringBinaryEncoding(const pddl_strips_t *strips,
                                         const pddl_mgroups_t *mgs,
                                         bor_iset_t *binfs)
{
    bor_iset_t *fact_to_mgroup;
    int fact_id, mgroup_id;

    fact_to_mgroup = BOR_CALLOC_ARR(bor_iset_t, strips->fact.fact_size);
    for (int mi = 0; mi < mgs->mgroup_size; ++mi){
        const pddl_mgroup_t *mg = mgs->mgroup + mi;
        BOR_ISET_FOR_EACH(&mg->fact, fact_id)
            borISetAdd(fact_to_mgroup + fact_id, mi);
    }

    for (int oi = 0; oi < strips->op.op_size; ++oi){
        const pddl_strips_op_t *op = strips->op.op[oi];
        BOR_ISET_FOR_EACH(&op->del_eff, fact_id){
            if (borISetIn(fact_id, &op->pre))
                continue;
            BOR_ISET_FOR_EACH(&fact_to_mgroup[fact_id], mgroup_id){
                if (borISetIsDisjunct(&mgs->mgroup[mgroup_id].fact,
                                      &op->add_eff)){
                    borISetAdd(binfs, fact_id);
                }
            }
        }
    }

    for (int i = 0; i < strips->fact.fact_size; ++i)
        borISetFree(fact_to_mgroup + i);
    if (fact_to_mgroup != NULL)
        BOR_FREE(fact_to_mgroup);
}

struct create {
    const pddl_strips_t *strips;
    int next_id; /*!< Next free ID for assignement */
    bor_iset_t *mgroup; /*!< Sorted mutex groups */
    int mgroup_size; /*< Number of non-empty mutex groups */
    bor_iset_t essential_facts;
};
typedef struct create create_t;

static void initEssentialFacts(create_t *c)
{
    int *fact_mgroups;

    fact_mgroups = BOR_CALLOC_ARR(int, c->strips->fact.fact_size);
    for (int i = 0; i < c->mgroup_size; ++i){
        int fact;
        BOR_ISET_FOR_EACH(&c->mgroup[i], fact)
            ++fact_mgroups[fact];
    }

    for (int f = 0; f < c->strips->fact.fact_size; ++f){
        if (fact_mgroups[f] == 1)
            borISetAdd(&c->essential_facts, f);
    }

    BOR_FREE(fact_mgroups);
}

static int cmpMGroup(const void *a, const void *b, void *_c)
{
    const create_t *c = _c;
    const bor_iset_t *i1 = a;
    const bor_iset_t *i2 = b;
    int ess1 = (borISetIsDisjunct(a, &c->essential_facts) ? 0 : 1);
    int ess2 = (borISetIsDisjunct(b, &c->essential_facts) ? 0 : 1);
    int cmp = ess2 - ess1;
    if (cmp == 0)
        cmp = borISetSize(i2) - borISetSize(i1);
    if (cmp == 0){
        for (int i = 0; i < i1->size; ++i){
            if (i1->s[i] != i2->s[i])
                return i1->s[i] - i2->s[i];
        }
    }
    return cmp;
}

static void createInit(create_t *c, const pddl_strips_t *strips,
                       const pddl_mgroups_t *mg, unsigned flags)
{
    bor_iset_t single_facts;
    bor_iset_t bin_facts;
    int fact_id;

    // Remember strips problem
    c->strips = strips;

    // Initialize ID counter
    c->next_id = 0;

    // single_facts contains fact IDs that are not covered by any mutex
    // group
    borISetInit(&single_facts);
    for (int i = 0; i < c->strips->fact.fact_size; ++i)
        borISetAdd(&single_facts, i);

    // Copy mutex groups into create_t structure and update single_facts in
    // the process
    c->mgroup_size = mg->mgroup_size;
    c->mgroup = BOR_ALLOC_ARR(bor_iset_t, c->mgroup_size);
    for (int i = 0; i < c->mgroup_size; ++i){
        borISetInit(c->mgroup + i);
        borISetUnion(c->mgroup + i, &mg->mgroup[i].fact);
        borISetMinus(&single_facts, &mg->mgroup[i].fact);
    }

    // Force binary encoding on facts that cannot be properly encoded with
    // the current operators and mutex groups
    borISetInit(&bin_facts);
    factsRequiringBinaryEncoding(strips, mg, &bin_facts);
    borISetUnion(&single_facts, &bin_facts);
    for (int i = 0; i < c->mgroup_size; ++i)
        borISetMinus(c->mgroup + i, &bin_facts);
    borISetFree(&bin_facts);

    // Create one mutex group for each fact that is not covered by the
    // input mutex groups. This way we do not need to have separate
    // procedure for the facts that need to be encoded in binary.
    if (borISetSize(&single_facts) > 0){
        c->mgroup = BOR_REALLOC_ARR(c->mgroup, bor_iset_t,
                                c->mgroup_size + borISetSize(&single_facts));
        BOR_ISET_FOR_EACH(&single_facts, fact_id){
            bor_iset_t *g = c->mgroup + c->mgroup_size++;
            borISetInit(g);
            borISetAdd(g, fact_id);
        }
    }
    borISetFree(&single_facts);

    borISetInit(&c->essential_facts);
    if (flags == PDDL_FDR_VARS_ESSENTIAL_FIRST)
        initEssentialFacts(c);

    // Sort mutex groups in descending order in their size
    borSort(c->mgroup, c->mgroup_size, sizeof(bor_iset_t), cmpMGroup, c);
}

static void createFree(create_t *c)
{
    for (int i = 0; i < c->mgroup_size; ++i)
        borISetFree(c->mgroup + i);
    if (c->mgroup != NULL)
        BOR_FREE(c->mgroup);
}

static int needNoneOfThoseOp(const bor_iset_t *group,
                             const pddl_strips_op_t *op)
{
    if (borISetIntersectionSizeAtLeast(group, &op->del_eff, 1)){
        if (borISetIntersectionSizeAtLeast(group, &op->add_eff, 1))
            return 0;
        return 1;
    }
    return 0;
}

static int needNoneOfThose(const bor_iset_t *group,
                           const pddl_strips_t *strips)
{
    if (!borISetIntersectionSizeAtLeast(group, &strips->init, 1))
        return 1;

    for (int i = 0; i < strips->op.op_size; ++i){
        if (needNoneOfThoseOp(group, strips->op.op[i]))
            return 1;
    }
    return 0;
}

static void createVarFromMGroup(pddl_fdr_vars_t *vars, create_t *c,
                                const bor_iset_t *group)
{
    pddl_fdr_var_t *var;
    pddl_fdr_val_t *val;
    int none_of_those;
    const char *name;

    // Allocate a new variable
    ++vars->size;
    vars->var = BOR_REALLOC_ARR(vars->var, pddl_fdr_var_t, vars->size);
    var = vars->var + vars->size - 1;
    pddlFDRVarInit(var);

    // Determine whether we need another value "none of those"
    if (group->size == 1){
        none_of_those = 1;
    }else{
        none_of_those = needNoneOfThose(group, c->strips);
    }

    // Allocate values
    var->size = group->size;
    if (none_of_those)
        var->size += 1;
    var->val = BOR_CALLOC_ARR(pddl_fdr_val_t, var->size);

    // Initialize each value
    for (int i = 0; i < group->size; ++i){
        val = var->val + i;
        name = c->strips->fact.fact[group->s[i]]->name;
        val->name = BOR_ALLOC_ARR(char, strlen(name) + 3);
        sprintf(val->name, "(%s)", name);
        val->var_id = vars->size - 1;
        val->val_id = i;
        val->id = c->next_id++;
        val->strips_fact_id = group->s[i];

        ASSERT(vars->strips_fact_id_to_val[val->strips_fact_id] == NULL);
        vars->strips_fact_id_to_val[val->strips_fact_id] = val;
    }

    // Add "none of those" value
    if (none_of_those){
        val = var->val + var->size - 1;
        val->name = BOR_STRDUP("<none of those>");
        val->var_id = vars->size - 1;
        val->val_id = var->size - 1;
        val->id = c->next_id++;
        val->strips_fact_id = -1;

        var->none_of_those = val;
    }
}

static void substractMGroup(create_t *c, const bor_iset_t *mg)
{
    for (int i = 0; i < c->mgroup_size; ++i)
        borISetMinus(c->mgroup + i, mg);
    borSort(c->mgroup, c->mgroup_size, sizeof(bor_iset_t), cmpMGroup, c);

    // Remove empty sets that appear at the end of the array
    for (; c->mgroup_size > 0 && c->mgroup[c->mgroup_size - 1].size == 0;
            --c->mgroup_size)
        borISetFree(c->mgroup + c->mgroup_size - 1);
}

static void init(pddl_fdr_vars_t *vars,
                 const pddl_strips_t *strips,
                 const pddl_mgroups_t *mg,
                 unsigned flags)
{
    bor_iset_t mgroup;
    create_t c;

    borISetInit(&mgroup);
    createInit(&c, strips, mg, flags);
    while (c.mgroup_size > 0){
        borISetEmpty(&mgroup);
        borISetUnion(&mgroup, c.mgroup + 0);
        createVarFromMGroup(vars, &c, &mgroup);
        substractMGroup(&c, &mgroup);
    }
    createFree(&c);
    borISetFree(&mgroup);
}

void pddlFDRVarsInit(pddl_fdr_vars_t *vars, const pddl_strips_t *strips,
                     const pddl_mgroups_t *mg, unsigned flags)
{
    bzero(vars, sizeof(*vars));
    vars->strips_fact_id_to_val = BOR_CALLOC_ARR(pddl_fdr_val_t *,
                                                 strips->fact.fact_size);
    init(vars, strips, mg, flags);
}

void pddlFDRVarsFree(pddl_fdr_vars_t *vars)
{
    for (int i = 0; i < vars->size; ++i)
        pddlFDRVarFree(vars->var + i);
    if (vars->var != NULL)
        BOR_FREE(vars->var);
    if (vars->strips_fact_id_to_val != NULL)
        BOR_FREE(vars->strips_fact_id_to_val);
}

void pddlFDRVarsPrint(const pddl_fdr_vars_t *vars, FILE *fout)
{
    const pddl_fdr_var_t *var;
    const pddl_fdr_val_t *val;

    for (int i = 0; i < vars->size; ++i){
        var = vars->var + i;
        fprintf(fout, "Var %d [%d]", i, var->size);
        if (var->none_of_those == NULL){
            fprintf(fout, ", N: -1");
        }else{
            fprintf(fout, ", N: %d", var->none_of_those->id);
        }
        fprintf(fout, ":\n");
        for (int j = 0; j < var->size; ++j){
            val = var->val + j;
            fprintf(fout, "  %d: \"%s\", id: %d, strips_fact_id: %d,"
                          " var_id: %d\n",
                          j, val->name, val->id, val->strips_fact_id,
                          val->var_id);
        }
    }
}
