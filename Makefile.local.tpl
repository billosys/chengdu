DEBUG = yes
BOR_CPLEX_CFLAGS = -I/opt/cplex1263/cplex/include
BOR_CPLEX_LDFLAGS = -L/opt/cplex1263/cplex/lib/x86-64_linux/static_pic/ -lcplex
BOR_GUROBI_CFLAGS = -I/opt/gurobi/include
BOR_GUROBI_LDFLAGS = -L/opt/gurobi/lib -Wl,-rpath=/opt/gurobi/lib -lgurobi70

