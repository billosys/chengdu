#ifndef __PDDL_CONFIG_H__
#define __PDDL_CONFIG_H__

ifdef(`DEBUG', `#define PDDL_DEBUG')
ifdef(`USE_CPLEX', `#define PDDL_USE_CPLEX')
ifdef(`USE_LP_SOLVE', `#define PDDL_USE_LP_SOLVE')
ifdef(`LP', `#define PDDL_LP')

#endif /* __PDDL_CONFIG_H__ */
