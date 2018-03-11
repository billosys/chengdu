#include <stdio.h>
#include <pddl/pddl.h>

int main(int argc, char *argv[])
{
    pddl_config_t cfg = PDDL_CONFIG_INIT;
    pddl_t *pddl;

    if (argc != 3){
        fprintf(stderr, "Usage; %s domain.pddl problem.pddl\n", argv[0]);
        return -1;
    }

    pddlErrSetWarnOutput(stderr);
    pddlErrSetInfoOutput(stderr);
    cfg.force_adl = 0;
    cfg.normalize = 1;
    cfg.strips = 1;
    cfg.strips_cfg.prune.enable = 0;
    cfg.strips_cfg.h_mutex = 0;
    cfg.strips_cfg.fa_mgroup = 0;
    cfg.fdr = 0;

    pddl = pddlNew(argv[1], argv[2], &cfg);
    if (pddl == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    pddlDel(pddl);
    return 0;
}
