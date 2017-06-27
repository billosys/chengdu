-include Makefile.local
-include Makefile.include

CFLAGS += -I.
CFLAGS += $(BORUVKA_CFLAGS)
CFLAGS += $(LP_CFLAGS)

CPPCHECK_FLAGS += --platform=unix64 --enable=all -I. -Ithird-party/boruvka

TARGETS  = libpddl.a

OBJS  = iset
OBJS += lisp
OBJS += require
OBJS += type
OBJS += param
OBJS += obj
OBJS += pred
OBJS += fact
OBJS += fact_id
OBJS += action
OBJS += prep_action
OBJS += pddl
OBJS += cond
OBJS += cond_arr
OBJS += strips
OBJS += strips_op
OBJS += strips_ground
OBJS += strips_reachability_graph
OBJS += h2_mutex

OBJS := $(foreach obj,$(OBJS),.objs/$(obj).o)

all: $(TARGETS)

libpddl.a: $(OBJS)
	ar cr $@ $(OBJS)
	ranlib $@

pddl/config.h: pddl/config.h.m4
	$(M4) $(CONFIG_FLAGS) $< >$@

pddl/iset.h: third-party/boruvka/boruvka/set_arr.h.m4
	$(M4) -DGUARD=__PDDL_ISET_H__ \
          -DTYPE=int \
          -DSTRUCT_NAME=pddl_iset \
          -DFUNC_PREFIX=pddlISet \
          -DFOR_EACH_NAME=PDDL_ISET_FOR_EACH \
          -DARR_NAME=s \
              <$< >$@
src/iset.c: third-party/boruvka/src/set_arr.c.m4 pddl/iset.h
	$(M4) -DTYPE=int \
          -DSTRUCT_NAME=pddl_iset \
          -DFUNC_PREFIX=pddlISet \
          -DARR_NAME=s \
          -DLT='(x) < (y)' \
          -DEQ='(x) == (y)' \
          -DHEADER_FILE=pddl/iset.h \
              <$< >$@

.objs/%.o: src/%.c pddl/%.h pddl/config.h
	$(CC) $(CFLAGS) -c -o $@ $<
.objs/%.o: src/%.c pddl/config.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.h: pddl/config.h
%.c: pddl/config.h


clean:
	rm -f $(OBJS)
	rm -f .objs/*.o
	rm -f $(TARGETS)
	rm -f pddl/config.h
	rm -f src/*.pb.{cc,h}
	rm -f src/iset.c pddl/iset.h
	if [ -d bin ]; then $(MAKE) -C bin clean; fi;
	if [ -d testsuites ]; then $(MAKE) -C testsuites clean; fi;
	if [ -d doc ]; then $(MAKE) -C doc clean; fi;

check:
	$(MAKE) -C testsuites check
check-valgrind:
	$(MAKE) -C testsuites check-valgrind
check-segfault:
	$(MAKE) -C testsuites check-segfault
static-check:
	$(CPPCHECK) $(CPPCHECK_FLAGS) pddl/ src/

doc:
	$(MAKE) -C doc

analyze: clean
	$(SCAN_BUILD) $(MAKE)

submodule:
	$(MAKE) -C third-party submodule
third-party:
	$(MAKE) -C third-party
third-party-clean:
	$(MAKE) -C third-party clean

.PHONY: all clean check check-valgrind help doc install analyze examples submodule third-party
