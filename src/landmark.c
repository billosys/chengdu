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
#include <boruvka/hfunc.h>
#include "pddl/landmark.h"


static bor_htable_key_t htableHash(const bor_list_t *k, void *_)
{
    const pddl_landmark_t *ldm = BOR_LIST_ENTRY(k, pddl_landmark_t, htable);
    return ldm->hash;
}

static int htableEq(const bor_list_t *k1, const bor_list_t *k2, void *_)
{
    const pddl_landmark_t *l1 = BOR_LIST_ENTRY(k1, pddl_landmark_t, htable);
    const pddl_landmark_t *l2 = BOR_LIST_ENTRY(k2, pddl_landmark_t, htable);

    if (l1->op.size != l2->op.size)
        return 0;

    for (int size = borISetSize(&l1->op), i = 0; i < size; ++i){
        if (borISetGet(&l1->op, i) != borISetGet(&l2->op, i))
            return 0;
    }
    return 1;
}

static bor_htable_key_t ldmHash(const pddl_landmark_t *ldm)
{
    return borCityHash_64(ldm->op.s, sizeof(int) * ldm->op.size);
}

static pddl_landmark_t *pddlLandmarkNew(const bor_iset_t *op)
{
    pddl_landmark_t *ldm = BOR_ALLOC(pddl_landmark_t);

    bzero(ldm, sizeof(*ldm));
    borISetInit(&ldm->op);
    borISetUnion(&ldm->op, op);
    borListInit(&ldm->htable);
    ldm->hash = ldmHash(ldm);
    ldm->cost = 0;
    return ldm;
}
    
static void pddlLandmarkDel(pddl_landmark_t *ldm)
{
    borISetFree(&ldm->op);
    BOR_FREE(ldm);
}

void pddlLandmarksInit(pddl_landmarks_t *ldms)
{
    bzero(ldms, sizeof(*ldms));
    ldms->htable = borHTableNew(htableHash, htableEq, NULL);

    ldms->ldm_alloc = 2;
    ldms->ldm = BOR_ALLOC_ARR(pddl_landmark_t *, ldms->ldm_alloc);
}

void pddlLandmarksFree(pddl_landmarks_t *ldms)
{
    if (ldms->htable != NULL)
        borHTableDel(ldms->htable);
    for (int i = 0; i < ldms->ldm_size; ++i)
        pddlLandmarkDel(ldms->ldm[i]);
    if (ldms->ldm != NULL)
        BOR_FREE(ldms->ldm);
}

pddl_landmark_t *pddlLandmarksAdd(pddl_landmarks_t *ldms, const bor_iset_t *op)
{
    pddl_landmark_t find, *ldm;
    bor_list_t *lfind;

    find.op = *op;
    find.hash = ldmHash(&find);
    if ((lfind = borHTableFind(ldms->htable, &find.htable)) != NULL){
        ldm = BOR_LIST_ENTRY(lfind, pddl_landmark_t, htable);
    }else{
        ldm = pddlLandmarkNew(op);

        if (ldms->ldm_size >= ldms->ldm_alloc){
            ldms->ldm_alloc *= 2;
            ldms->ldm = BOR_REALLOC_ARR(ldms->ldm, pddl_landmark_t *,
                                        ldms->ldm_alloc);
        }
        ldms->ldm[ldms->ldm_size] = ldm;
        ldm->id = ldms->ldm_size++;
        borHTableInsert(ldms->htable, &ldm->htable);
    }

    return ldm;
}

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


void pddlLandmarkSeqInit(pddl_landmark_seq_t *lseq)
{
    bzero(lseq, sizeof(*lseq));
}

void pddlLandmarkSeqFree(pddl_landmark_seq_t *lseq)
{
    for (int i = 0; i < lseq->ldm_size; ++i)
        borISetFree(lseq->ldm + i);
    if (lseq->ldm != NULL)
        BOR_FREE(lseq->ldm);
}

void pddlLandmarkSeqCopy(pddl_landmark_seq_t *dst,
                         const pddl_landmark_seq_t *src)
{
    for (int i = 0; i < src->ldm_size; ++i)
        pddlLandmarkSeqAppend(dst, src->ldm + i);
}

static void landmarkSeqMakeSpace(pddl_landmark_seq_t *lseq)
{
    if (lseq->ldm_size == lseq->ldm_alloc){
        if (lseq->ldm_alloc == 0)
            lseq->ldm_alloc = 1;
        lseq->ldm_alloc *= 2;
        lseq->ldm = BOR_REALLOC_ARR(lseq->ldm, bor_iset_t, lseq->ldm_alloc);
        for (int i = lseq->ldm_size; i < lseq->ldm_alloc; ++i)
            borISetInit(lseq->ldm + i);
    }
}

void pddlLandmarkSeqAppend(pddl_landmark_seq_t *lseq, const bor_iset_t *ldm)
{
    landmarkSeqMakeSpace(lseq);
    borISetEmpty(lseq->ldm + lseq->ldm_size);
    borISetUnion(lseq->ldm + lseq->ldm_size, ldm);
    ++lseq->ldm_size;
}

void pddlLandmarkSeqPrepend(pddl_landmark_seq_t *lseq, const bor_iset_t *ldm)
{
    landmarkSeqMakeSpace(lseq);
    for (int i = lseq->ldm_size; i > 0; --i)
        lseq->ldm[i] = lseq->ldm[i - 1];
    borISetInit(lseq->ldm + 0);
    borISetUnion(lseq->ldm + 0, ldm);
    ++lseq->ldm_size;
}

static int landmarkSeqPruneOpBeforeAfter(pddl_landmark_seq_t *lseq,
                                          int ldm_id,
                                          int op_id)
{
    int change = 0, from, to;

    for (to = ldm_id - 1; to >= 0 && borISetIn(op_id, lseq->ldm + to); --to);
    for (int i = 0; i <= to; ++i){
        int size = borISetSize(lseq->ldm + i);
        borISetRm(lseq->ldm + i, op_id);
        change |= (size != borISetSize(lseq->ldm + i));
    }


    for (from = ldm_id + 1;
            from < lseq->ldm_size && borISetIn(op_id, lseq->ldm + from);
            ++from);
    for (int i = from; i < lseq->ldm_size; ++i){
        int size = borISetSize(lseq->ldm + i);
        borISetRm(lseq->ldm + i, op_id);
        change |= (size != borISetSize(lseq->ldm + i));
    }
    return change;
}

void pddlLandmarkSeqPruneOnlyOnceOps(pddl_landmark_seq_t *lseq,
                                     const bor_iset_t *only_once_ops)
{
    int change = 1;

    while (change) {
        change = 0;
        for (int i = 0; i < lseq->ldm_size; ++i){
            const bor_iset_t *ldm = lseq->ldm + i;
            if (borISetSize(ldm) == 1 && borISetIsSubset(ldm, only_once_ops)){
                int op = borISetGet(ldm, 0);
                change |= landmarkSeqPruneOpBeforeAfter(lseq, i, op);
            }
        }
    }
}
