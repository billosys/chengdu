#include <pddl/pddl.h>

#include "options.h"

int main(int argc, char *argv[])
{
    options_t *o;
    pddl_config_t cfg;
    pddl_t *pddl;

    if ((o = options(argc, argv)) == NULL)
        return -1;

    cfg = o->cfg;
    cfg.force_adl = 1;
    cfg.strips = 1;
    cfg.strips_cfg.prune.enable = 1;
    cfg.strips_cfg.fa_mgroup = 1;
    cfg.fdr = 1;
    pddl = pddlNew(o->domain_pddl, o->problem_pddl, &cfg);
    if (pddl == NULL){
        pddlErrPrintWithTraceback();
        return -1;
    }

    if (o->output_type == OUTPUT_FD){
        pddlFDRPrintAsFD(pddl->fdr, stdout);
    }else if (o->output_type == OUTPUT_STRIPS){
        pddlStripsDump(pddl->strips, stdout);
    }else{
        pddlStripsPrintPython(pddl->strips, stdout);
    }

    pddlDel(pddl);
    return 0;
}
