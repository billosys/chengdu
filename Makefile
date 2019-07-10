-include Makefile.local
-include Makefile.include

CFLAGS += -I.
CFLAGS += $(BORUVKA_CFLAGS)

CPPCHECK_FLAGS += --platform=unix64 --enable=all -I. -Ithird-party/boruvka

TARGETS  = libpddl.a

OBJS  = lisp
OBJS += require
OBJS += type
OBJS += param
OBJS += obj
OBJS += pred
OBJS += fact
OBJS += action
OBJS += prep_action
OBJS += pddl
OBJS += cond
OBJS += cond_arr
OBJS += strips
OBJS += strips_op
OBJS += strips_fact_cross_ref
OBJS += strips_ground_tree
OBJS += strips_ground
OBJS += action_args
OBJS += ground_atom
OBJS += profile
OBJS += helper
OBJS += lifted_mgroup
OBJS += lifted_mgroup_infer
OBJS += lifted_mgroup_htable
OBJS += ground_mgroup
OBJS += pddl_file
OBJS += irrelevance

OBJS := $(foreach obj,$(OBJS),.objs/$(obj).o)

all: $(TARGETS)

libpddl.a: $(OBJS)
	ar cr $@ $(OBJS)
	ranlib $@

pddl/config.h:
	echo "#ifndef __PDDL_CONFIG_H__" >$@
	echo "#define __PDDL_CONFIG_H__" >>$@
	echo "" >>$@
	if [ "$(DEBUG)" = "yes" ]; then echo "#define PDDL_DEBUG" >>$@; fi
	echo "" >>$@
	echo "#endif /* __PDDL_CONFIG_H__ */" >>$@

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
	if [ -d bin ]; then $(MAKE) -C bin clean; fi;
	if [ -d test ]; then $(MAKE) -C test clean; fi;
	if [ -d doc ]; then $(MAKE) -C doc clean; fi;

mrproper: clean boruvka-clean opts-clean

check:
	$(MAKE) -C test check
check-ci:
	$(MAKE) -C test check-ci
check-valgrind:
	$(MAKE) -C test check-valgrind
check-segfault:
	$(MAKE) -C test check-segfault
static-check:
	$(CPPCHECK) $(CPPCHECK_FLAGS) pddl/ src/

doc:
	$(MAKE) -C doc

analyze: clean
	$(SCAN_BUILD) $(MAKE)

third-party: boruvka opts
third-party-clean: boruvka-clean opts-clean

boruvka: third-party/boruvka/Makefile
	$(MAKE) $(_BOR_MAKE_DEF) -C third-party/boruvka all
boruvka-clean:
	$(MAKE) -C third-party/boruvka clean
third-party/boruvka/Makefile:
	git submodule init -- third-party/boruvka
	git submodule update -- third-party/boruvka

opts: third-party/opts/Makefile
	$(MAKE) -C third-party/opts all
opts-clean:
	$(MAKE) -C third-party/opts clean
third-party/opts/Makefile:
	git submodule init -- third-party/opts
	git submodule update -- third-party/opts

.PHONY: all clean check check-ci check-valgrind help doc install analyze examples third-party third-party-clean boruvka boruvka-clean opts opts-clean mrproper
