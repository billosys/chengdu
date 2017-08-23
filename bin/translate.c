#include <pddl/pddl.h>

int main(int argc, char *argv[])
{
    pddl_config_t cfg = PDDL_CONFIG_INIT;
    pddl_t *pddl;

    if (argc != 3){
        fprintf(stderr, "Usage: %s domain.pddl problem.pddl\n", argv[0]);
        return -1;
    }

    cfg.force_adl = 1;
    cfg.normalize = 1;
    cfg.compile_away_cond_eff = 0;
    cfg.strips = 1;
    cfg.fdr = 1;
    pddl = pddlNew(argv[1], argv[2], &cfg);
    if (pddl == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    pddlFDRPrintAsFD(pddl->fdr, stdout);
    pddlDel(pddl);
    return 0;
}
