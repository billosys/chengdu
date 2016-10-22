-include Makefile.local
-include Makefile.include

CFLAGS += -I.
CFLAGS += $(BORUVKA_CFLAGS)
CFLAGS += $(LP_CFLAGS)

TARGETS  = libplan.a

OBJS  = problem
OBJS += problem_fd
OBJS += var
OBJS += state
OBJS += part_state
OBJS += state_packer
OBJS += state_pool
OBJS += op
OBJS += op_id_tr
OBJS += succ_gen
OBJS += causal_graph
OBJS += path
OBJS += state_space
OBJS += prio_queue
OBJS += landmark
OBJS += fact_id
OBJS += list_lazy
OBJS += list_lazy_fifo
OBJS += list_lazy_heap
OBJS += list_lazy_bucket
OBJS += list_lazy_rbtree
OBJS += list_lazy_splaytree
OBJS += list
OBJS += list_tiebreaking
OBJS += search
OBJS += search_applicable_ops
OBJS += search_stat
OBJS += search_lazy_base
OBJS += search_ehc
OBJS += search_lazy
OBJS += search_astar
OBJS += heur
OBJS += dtg
OBJS += fact_op_cross_ref
OBJS += pref_op_selector
OBJS += heur_relax
OBJS += heur_goalcount
OBJS += heur_relax_add_max
OBJS += heur_relax_ff
OBJS += heur_lm_cut
OBJS += heur_dtg
OBJS += heur_flow
OBJS += heur_potential
OBJS += heur_ma_ff
OBJS += heur_ma_dtg
OBJS += heur_ma_max
OBJS += heur_ma_lm_cut
OBJS += heur_ma_pot
OBJS += heur_ma_pot_proj
OBJS += msg_schema
OBJS += ma_msg
OBJS += ma_comm_nanomsg
OBJS += ma_search
OBJS += ma_snapshot
OBJS += ma_private_state
OBJS += ma_state
OBJS += lp
OBJS += pot

OBJS := $(foreach obj,$(OBJS),.objs/$(obj).o)

all: $(TARGETS)

libpddl.a: $(OBJS)
	ar cr $@ $(OBJS)
	ranlib $@

cpddl/config.h: cpddl/config.h.m4
	$(M4) $(CONFIG_FLAGS) $< >$@

.objs/%.o: src/%.c cpddl/%.h cpddl/config.h
	$(CC) $(CFLAGS) -c -o $@ $<
.objs/%.o: src/%.c cpddl/config.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.h: cpddl/config.h
%.c: cpddl/config.h


clean:
	rm -f $(OBJS)
	rm -f .objs/*.o
	rm -f $(TARGETS)
	rm -f cpddl/config.h
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
