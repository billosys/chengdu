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

#ifndef __PDDL_LANDMARK_H__
#define __PDDL_LANDMARK_H__

#include <boruvka/iset.h>
#include <boruvka/htable.h>
#include <pddl/common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct pddl_landmark {
    bor_iset_t op; /*!< Set of operators */
    int cost;

    int id;
    bor_list_t htable;
    bor_htable_key_t hash;
};
typedef struct pddl_landmark pddl_landmark_t;

struct pddl_landmarks {
    bor_htable_t *htable;
    pddl_landmark_t **ldm;
    int ldm_size;
    int ldm_alloc;
};
typedef struct pddl_landmarks pddl_landmarks_t;

void pddlLandmarksInit(pddl_landmarks_t *ldms);
void pddlLandmarksFree(pddl_landmarks_t *ldms);

/**
 * Adds a new landmark consisting of the given set of operators if not
 * already there.
 */
pddl_landmark_t *pddlLandmarksAdd(pddl_landmarks_t *ldms, const bor_iset_t *op);


struct pddl_disjunctive_landmarks {
    bor_iset_t *ldm;
    int ldm_size;
    int ldm_alloc;
};
typedef struct pddl_disjunctive_landmarks pddl_disjunctive_landmarks_t;

void pddlDisjunctiveLandmarksInit(pddl_disjunctive_landmarks_t *ldms);
void pddlDisjunctiveLandmarksFree(pddl_disjunctive_landmarks_t *ldms);
void pddlDisjunctiveLandmarksAdd(pddl_disjunctive_landmarks_t *ldms,
                                 const bor_iset_t *ldm);


/**
 * Sequence of landmarks.
 */
struct pddl_landmark_seq {
    bor_iset_t *ldm;
    int ldm_size;
    int ldm_alloc;
};
typedef struct pddl_landmark_seq pddl_landmark_seq_t;

/**
 * TODO
 */
void pddlLandmarkSeqInit(pddl_landmark_seq_t *lseq);

/**
 * TODO
 */
void pddlLandmarkSeqFree(pddl_landmark_seq_t *lseq);

/**
 * TODO
 */
void pddlLandmarkSeqCopy(pddl_landmark_seq_t *dst,
                         const pddl_landmark_seq_t *src);

/**
 * TODO
 */
void pddlLandmarkSeqAppend(pddl_landmark_seq_t *lseq, const bor_iset_t *ldm);

/**
 * TODO
 */
void pddlLandmarkSeqPrepend(pddl_landmark_seq_t *lseq, const bor_iset_t *ldm);

_bor_inline void pddlLandmarkSeqEmpty(pddl_landmark_seq_t *lseq)
{
    lseq->ldm_size = 0;
}

/**
 * TODO
 */
void pddlLandmarkSeqPruneOnlyOnceOps(pddl_landmark_seq_t *lseq,
                                     const bor_iset_t *only_once_ops);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_LANDMARK_H__ */
