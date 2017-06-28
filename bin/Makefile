-include ../Makefile.local
-include ../Makefile.include

CFLAGS += -I../
CFLAGS += $(BORUVKA_CFLAGS)
CFLAGS += $(OPTS_CFLAGS)
LDFLAGS += -L../ -lpddl
LDFLAGS += $(BORUVKA_LDFLAGS)
LDFLAGS += $(OPTS_LDFLAGS)
LDFLAGS += -pthread -lrt -lm

ifeq '$(BIN_STATIC)' 'yes'
  LDFLAGS += -static
endif

OBJS  = search.o
OBJS += options.o

TARGETS = pddl-check

all: $(TARGETS)


search: ../libpddl.a $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

pddl-%: %.c ../libpddl.a
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGETS)
	rm -f *.o

.PHONY: all
