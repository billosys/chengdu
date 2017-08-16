#include "pddl/pddl.h"
#include "pddl/strips.h"
#include "pddl/fdr.h"
#include "pddl/err.h"

int main(int argc, char *argv[])
{
    pddl_config_t cfg = PDDL_CONFIG_INIT;
    pddl_strips_prune_config_t prune_cfg = PDDL_STRIPS_PRUNE_CONFIG_INIT;
    pddl_t *pddl;
    pddl_strips_t *strips;
    pddl_mgroups_t *mgroups;
    pddl_fdr_t *fdr;

    if (argc != 3){
        fprintf(stderr, "Usage: %s domain.pddl problem.pddl\n", argv[0]);
        return -1;
    }

    cfg.force_adl = 1;
    cfg.normalize = 1;
    cfg.compile_away_cond_eff = 0;
    pddl = pddlNew(argv[1], argv[2], &cfg);
    if (pddl == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    strips = pddlStripsGround(pddl, 0);
    if (strips == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    pddlStripsPrune(strips, &prune_cfg);

    mgroups = pddlMGroupFindFA(strips);
    if (mgroups == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    fdr = pddlFDRFromStrips(strips, mgroups, 0);
    if (fdr == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    pddlFDRPrintAsFD(fdr, stdout);

    pddlFDRDel(fdr);
    pddlMGroupsDel(mgroups);
    pddlStripsDel(strips);
    pddlDel(pddl);
    return 0;
}
