#include <pddl/pddl.h>
#include <pddl/sync_product.h>

size_t max_mem = 7ul * 1024ul * 1024ul * 1024ul;

static int goalDistance(pddl_sync_product_t *sp)
{
    int init_node = pddlSyncProductInitNode(sp);
    int dist;

    BOR_IARR(goal_dist);
    pddlSyncProductGoalDistance(sp, &goal_dist);
    dist = borIArrGet(&goal_dist, init_node);
    //printf("  Goal Distance: %d\n", dist);
    borIArrFree(&goal_dist);

    return dist;
}

static int ldmDistance(pddl_sync_product_t *sp,
                       const pddl_strips_cross_ref_t *cref)

{
    int init_node = pddlSyncProductInitNode(sp);
    int cost = -1;

    if (pddlSyncProductFindLandmarks(sp, cref, init_node,
                                     NULL, NULL, NULL, &cost) != 0)
        pddlErrPrintWithTraceback();
    return cost;
}

static int hasGoal(const pddl_strips_t *strips,
                   const bor_iset_t *mgroups)
{
    int mi;
    BOR_ISET_FOR_EACH(mgroups, mi){
        if (strips->mgroup.mgroup[mi].is_goal)
            return 1;
    }
    return 0;
}

static int mgroupsRelated(const bor_iset_t *mg,
                          int new_mg,
                          const pddl_strips_cross_ref_t *cref)
{
    int mi;

    if (borISetSize(mg) <= 1)
        return 1;

    BOR_ISET_FOR_EACH(mg, mi){
        if (mi != new_mg
                && !borISetIsDisjunct(&cref->mgroup[mi].op,
                                      &cref->mgroup[new_mg].op)){
            return 1;
        }
    }
    return 0;
}

static void syncProduct(const pddl_strips_t *strips,
                        const pddl_strips_cross_ref_t *cref,
                        int start_mi,
                        bor_iset_t *mgroups,
                        int prev_goal_dist)
{
    int mgi, dist = 0, ldm_cost = 0;

    for (int mi = start_mi; mi < strips->mgroup.mgroup_size; ++mi){
        borISetAdd(mgroups, mi);

        dist = prev_goal_dist;
        if (mgroupsRelated(mgroups, mi, cref)
                && pddlSyncProductCanFitInMem(mgroups, strips, max_mem)
                && hasGoal(strips, mgroups)){

            pddl_sync_product_t sp;
            if (pddlSyncProductInit(&sp, mgroups, strips, cref) == 0){
                dist = goalDistance(&sp);
                ldm_cost = ldmDistance(&sp, cref);

                printf("SP:");
                BOR_ISET_FOR_EACH(mgroups, mgi)
                    printf(" %d", mgi);
                printf(" --> %d, %d", dist, ldm_cost);
                printf("\n");

                if (dist != ldm_cost){
                    printf("XXX: dist != ldm_cost\n");
                }

                if (dist < prev_goal_dist){
                    printf("XXX!!!\n");
                }

                pddlSyncProductFree(&sp);
            }else{
                pddlErrPrintWithTraceback();
            }

        }

        if (mgroupsRelated(mgroups, mi, cref)
                && pddlSyncProductCanFitInMem(mgroups, strips, max_mem)){
            syncProduct(strips, cref, mi + 1, mgroups, dist);
        }
        borISetRm(mgroups, mi);
    }
}

int main(int argc, char *argv[])
{
    pddl_config_t cfg = PDDL_CONFIG_INIT;
    pddl_t *pddl;
    pddl_strips_t *strips;

    if (argc != 3){
        fprintf(stderr, "Usage: %s domain.pddl problem.pddl\n", argv[0]);
        return -1;
    }

    pddlErrSetWarnOutput(stderr);
    pddlErrSetInfoOutput(stderr);

    cfg.force_adl = 1;
    cfg.normalize = 1;
    cfg.compile_away_cond_eff = 1;
    cfg.strips = 1;
    cfg.strips_cfg.h_mutex = 2;
    cfg.strips_cfg.fa_mgroup = 1;
    cfg.strips_cfg.prune.enable = 1;
    cfg.strips_cfg.prune.fixpoint = 1;
    cfg.strips_cfg.prune.static_facts = 1;
    cfg.strips_cfg.prune.irrelevance = 1;
    cfg.strips_cfg.prune.h_mutex = 2;
    cfg.strips_cfg.prune.fa_mgroup = 1;
    cfg.strips_cfg.prune.fa_mgroup_dead_end = 1;
    cfg.strips_cfg.prune.disambiguation = 1;

    pddl = pddlNew(argv[1], argv[2], &cfg);
    if (pddl == NULL || pddl->strips == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    pddl_strips_cross_ref_t cref;
    BOR_ISET(mgroups);

    strips = pddl->strips;
    pddlStripsCompleteMGroups(strips);
    pddlStripsMakeExactlyOneMGroups(strips);
    pddlStripsCrossRefInit(&cref, strips);

    for (int i = 0; i < strips->op.op_size; ++i){
        BOR_ISET(mgs);
        int fact;
        BOR_ISET_FOR_EACH(&strips->op.op[i]->pre, fact)
            borISetUnion(&mgs, &cref.fact[fact].mgroup);
        printf("MG:");
        BOR_ISET_FOR_EACH(&mgs, fact)
            printf(" %d:%d", fact, strips->mgroup.mgroup[fact].is_goal);
        if (pddlSyncProductCanFitInMem(&mgs, strips, max_mem))
            printf(" C");
        printf("\n");
        borISetFree(&mgs);
    }

    /*
    syncProduct(strips, &cref, 0, &mgroups, 0);
    for (int mi = 0; mi < strips->mgroup.mgroup_size; ++mi){
        for (int mi2 = mi + 1; mi2 < strips->mgroup.mgroup_size; ++mi2){
            if (!strips->mgroup.mgroup[mi].is_goal
                    && !strips->mgroup.mgroup[mi2].is_goal){
                printf("Skipping %d x %d -- no goal\n", mi, mi2);
                continue;
            }
            borISetEmpty(&mgroups);
            borISetAdd(&mgroups, mi);
            borISetAdd(&mgroups, mi2);
            if (!pddlSyncProductCanFitInMem(&mgroups, strips, max_mem)){
                printf("Skipping %d x %d -- not enough memory\n", mi, mi2);
                continue;
            }

            printf("Sync product %d x %d\n", mi, mi2);
            pddl_sync_product_t sp;
            if (pddlSyncProductInit(&sp, &mgroups, strips, &cref) != 0)
                pddlErrPrintWithTraceback();

            goalDistance(&sp);

            pddlSyncProductFree(&sp);

            for (int mi3 = mi2 + 1; mi3 < strips->mgroup.mgroup_size; ++mi3){
                borISetAdd(&mgroups, mi3);
                if (!pddlSyncProductCanFitInMem(&mgroups, strips, max_mem)){
                    printf("Skipping %d x %d x %d -- not enough memory\n",
                           mi, mi2, mi3);
                    continue;
                }

                printf("Sync product %d x %d x %d\n", mi, mi2, mi3);
                if (pddlSyncProductInit(&sp, &mgroups, strips, &cref) != 0)
                    pddlErrPrintWithTraceback();

                goalDistance(&sp);

                pddlSyncProductFree(&sp);
            }
        }
    }
    */
    borISetFree(&mgroups);

    pddlDel(pddl);
    return 0;
}
