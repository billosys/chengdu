#include <stdio.h>
#include <pddl/pddl.h>

int main(int argc, char *argv[])
{
    pddl_t *pddl;

    if (argc != 3){
        fprintf(stderr, "Usage; %s domain.pddl problem.pddl\n", argv[0]);
        return -1;
    }

    pddl = pddlNew(argv[1], argv[2]);
    if (pddl == NULL)
        return -1;

    pddlDump(pddl, stdout);
    pddlDel(pddl);

    return 0;
}
