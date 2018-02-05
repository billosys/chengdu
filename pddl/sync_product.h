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

#ifndef __PDDL_SYNC_PRODUCT_H__
#define __PDDL_SYNC_PRODUCT_H__

#include <boruvka/iarr.h>
#include <pddl/strips.h>
#include <pddl/strips_cross_ref.h>
#include <pddl/landmark.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct pddl_sync_product_edge {
    int cost; /*!< Cost of the edge */
    int cur_cost; /*!< Temporary cost used for algorithms */
} bor_packed;
typedef struct pddl_sync_product_edge pddl_sync_product_edge_t;

struct pddl_sync_product_node {
    int *fact; /*!< Facts the state consists of */
    pddl_sync_product_edge_t *next; /*!< Edges starting in this node */
    //pddl_sync_product_edge_t *prev; /*!< Edges ending in this node */
    int *prev;
    int ldm_level;
    unsigned char is_goal:1; /*!< True if this is a goal state */
    unsigned char is_init:1; /*!< True if this is the initial state */
    unsigned char is_goal_zone:1;
    unsigned char is_init_zone:1;
} bor_packed;
typedef struct pddl_sync_product_node pddl_sync_product_node_t;

struct pddl_sync_product {
    void *mem; /*!< Allocated block of memory the whole graph is contained in */
    size_t mem_size;
    pddl_sync_product_node_t *node;
    int node_size;
    int fact_size;
    int has_goal;
    pddl_landmark_seq_t ldm_seq;
} bor_packed;
typedef struct pddl_sync_product pddl_sync_product_t;


#define PDDL_SYNC_PRODUCT_FOR_EACH_NEXT(SP, NODE, EDGEI) \
    for (int EDGEI = 0; EDGEI < (SP)->node_size; ++EDGEI) \
        if ((NODE)->next[EDGEI].cost < 0){ \
            EDGEI -= (NODE)->next[EDGEI].cost + 1; \
        }else

#define PDDL_SYNC_PRODUCT_FOR_EACH_PREV(SP, TOI, FROMI) \
    for (int FROMI = 0; FROMI < (SP)->node_size; ++FROMI) \
        if ((SP)->node[TOI].prev[FROMI] < 0){ \
            FROMI -= (SP)->node[TOI].prev[FROMI] + 1; \
        }else

/**
 * Returns the maximum number of nodes the synchronized product of the
 * given mutex groups would contain.
 */
size_t pddlSyncProductMaxNodes(const bor_iset_t *mgroups,
                               const pddl_strips_t *strips);

/**
 * Returns number of bytes required for storing the synchronized product of
 * the given mutex groups.
 */
size_t pddlSyncProductRequiredMem(const bor_iset_t *mgroups,
                                  const pddl_strips_t *strips);

/**
 * Returns true if the specified synchronized product can fit in the
 * specified memory.
 */
int pddlSyncProductCanFitInMem(const bor_iset_t *mgroups,
                               const pddl_strips_t *strips,
                               size_t mem);

/**
 * Returns true if the synchronized product consisting of num_nodes nodes,
 * each node having num_node_facts facts, can fit in the specified memory.
 */
int pddlSyncProductCanFitInMem2(size_t num_nodes, size_t num_node_facts,
                                size_t mem);

/**
 * TODO
 */
int pddlSyncProductLdmCanFitInMem(const bor_iset_t *mgroups,
                                  const pddl_strips_t *strips,
                                  int num_landmarks,
                                  size_t mem);

/**
 * Creates a synchronized product of the given mutex groups.
 * TODO
 */
int pddlSyncProductInit(pddl_sync_product_t *sprod,
                        const bor_iset_t *mgroups,
                        const pddl_strips_t *strips,
                        const pddl_strips_cross_ref_t *cross_ref);

/**
 * TODO
 */
int pddlSyncProductInitMaxNodes(pddl_sync_product_t *sprod,
                                const bor_iset_t *mgroups,
                                const pddl_strips_t *strips,
                                const pddl_strips_cross_ref_t *cross_ref,
                                int max_nodes);

/**
 * TODO
 */
int pddlSyncProductInitLdm(pddl_sync_product_t *sprod,
                           const pddl_strips_t *strips,
                           const pddl_strips_cross_ref_t *cross_ref,
                           const bor_iset_t *mgroups,
                           const pddl_landmark_seq_t *lseq);

/**
 * Frees allocated memory.
 */
void pddlSyncProductFree(pddl_sync_product_t *sprod);

/**
 * Returns a set of operators that correspond to the edge between the
 * specified nodes.
 * The operators are appended to the given set.
 */
void pddlSyncProductEdgeOps(const pddl_sync_product_t *sprod,
                            const pddl_strips_cross_ref_t *cref,
                            int from_node, int to_node,
                            bor_iset_t *ops);

/**
 * Returns ID for the node corresponding to the initial state.
 */
int pddlSyncProductInitNode(const pddl_sync_product_t *sprod);

/**
 * Computes the shortest distance from each node to the nearest goal node.
 * If there is no path, -1 is used instead of distance.
 */
void pddlSyncProductGoalDistance(const pddl_sync_product_t *sprod,
                                 bor_iarr_t *dist);


/**
 * Finds landmarks from the given node.
 * TODO
 */
int pddlSyncProductFindLandmarks(pddl_sync_product_t *sprod,
                                 const pddl_strips_cross_ref_t *cref,
                                 int init_node,
                                 pddl_landmarks_t *ldms,
                                 pddl_landmark_seq_t *lseq,
                                 bor_iset_t *ldm_union,
                                 int *ldm_cost);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_SYNC_PRODUCT_H__ */
