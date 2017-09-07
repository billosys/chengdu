#include <pddl/pddl.h>

#include "options.h"

static int outputPDDLDomain(const options_t *o, const pddl_t *pddl)
{
    FILE *fout;

    fout = fopen(o->output_pddl_domain, "w");
    if (fout == NULL){
        fprintf(stderr, "Error: Could not open file %s.\n",
                o->output_pddl_domain);
        return -1;
    }
    pddlPrintPDDLDomain(pddl, fout);

    fclose(fout);
    return 0;
}

static int outputPDDLProblem(const options_t *o, const pddl_t *pddl)
{
    FILE *fout;

    fout = fopen(o->output_pddl_problem, "w");
    if (fout == NULL){
        fprintf(stderr, "Error: Could not open file %s.\n",
                o->output_pddl_problem);
        return -1;
    }
    pddlPrintPDDLProblem(pddl, fout);

    fclose(fout);
    return 0;
}

static int outputStripsPDDLDomain(const options_t *o, const pddl_t *pddl)
{
    FILE *fout;

    fout = fopen(o->output_strips_pddl_domain, "w");
    if (fout == NULL){
        fprintf(stderr, "Error: Could not open file %s.\n",
                o->output_strips_pddl_domain);
        return -1;
    }
    pddlStripsPrintPDDLDomain(pddl->strips, fout);

    fclose(fout);
    return 0;
}

static int outputStripsPDDLProblem(const options_t *o, const pddl_t *pddl)
{
    FILE *fout;

    fout = fopen(o->output_strips_pddl_problem, "w");
    if (fout == NULL){
        fprintf(stderr, "Error: Could not open file %s.\n",
                o->output_strips_pddl_problem);
        return -1;
    }
    pddlStripsPrintPDDLProblem(pddl->strips, fout);

    fclose(fout);
    return 0;
}

int main(int argc, char *argv[])
{
    options_t *o;
    pddl_config_t cfg;
    pddl_t *pddl;

    if ((o = options(argc, argv)) == NULL)
        return -1;

    if (o->quiet){
        pddlErrEnableWarn(0);
        pddlErrEnableInfo(0);
    }else{
        pddlErrSetWarnOutput(stderr);
        pddlErrSetInfoOutput(stderr);
    }

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

    if (o->output_pddl_domain != NULL){
        if (outputPDDLDomain(o, pddl) != 0)
            return -1;
    }
    if (o->output_pddl_problem != NULL){
        if (outputPDDLProblem(o, pddl) != 0)
            return -1;
    }
    if (o->output_strips_pddl_domain != NULL){
        if (outputStripsPDDLDomain(o, pddl) != 0)
            return -1;
    }
    if (o->output_strips_pddl_problem != NULL){
        if (outputStripsPDDLProblem(o, pddl) != 0)
            return -1;
    }

    pddlDel(pddl);
    return 0;
}
