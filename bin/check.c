#include <stdio.h>
#include <pddl/pddl.h>

int main(int argc, char *argv[])
{
    pddl_config_t cfg = PDDL_CONFIG_INIT;
    pddl_strips_config_t strips_cfg = PDDL_STRIPS_CONFIG_INIT;
    pddl_t pddl;
    pddl_strips_t *strips;

    if (argc != 3){
        fprintf(stderr, "Usage: %s domain.pddl problem.pddl\n", argv[0]);
        return -1;
    }

    pddlErrSetWarnOutput(stderr);
    pddlErrSetInfoOutput(stderr);
    cfg.force_adl = 0; // TODO: parametrize
    if (pddlInit(&pddl, argv[1], argv[2], &cfg) != 0){
        pddlErrPrintWithTraceback();
        return -1;
    }

    pddlNormalize(&pddl);
    strips = pddlStripsNew(&pddl, &strips_cfg);
    if (strips == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    pddlStripsDel(strips);
    pddlFree(&pddl);
    return 0;
}
