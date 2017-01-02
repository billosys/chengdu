-include Makefile.local
-include Makefile.include

CFLAGS += -I.
CFLAGS += $(BORUVKA_CFLAGS)
CFLAGS += $(LP_CFLAGS)

TARGETS  = libpddl.a

OBJS  = lisp
OBJS += require
OBJS += type
OBJS += param
OBJS += obj
OBJS += pred
OBJS += fact
OBJS += action
OBJS += pddl
OBJS += cond
OBJS += strips

OBJS := $(foreach obj,$(OBJS),.objs/$(obj).o)

all: $(TARGETS)

libpddl.a: $(OBJS)
	ar cr $@ $(OBJS)
	ranlib $@

pddl/config.h: pddl/config.h.m4
	$(M4) $(CONFIG_FLAGS) $< >$@

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
	if [ -d testsuites ]; then $(MAKE) -C testsuites clean; fi;
	if [ -d doc ]; then $(MAKE) -C doc clean; fi;

check:
	$(MAKE) -C testsuites check
check-valgrind:
	$(MAKE) -C testsuites check-valgrind
check-segfault:
	$(MAKE) -C testsuites check-segfault

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
