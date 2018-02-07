#include <pddl/pddl.h>
#include <pddl/sync_product.h>
#include <pddl/heur_mgroup_merge.h>

size_t max_mem = 4ul * 1024ul * 1024ul * 1024ul;

int main(int argc, char *argv[])
{
    pddl_config_t cfg = PDDL_CONFIG_INIT;
    pddl_t *pddl;

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

    pddl_heur_mgroup_merge_t heur;
    pddlHeurMGroupMergeInit(&heur, pddl->strips, 0, max_mem);
    pddlHeurMGroupMergeFree(&heur);

    pddlDel(pddl);
    return 0;
}
