-include ../Makefile.local
-include ../Makefile.include

all: boruvka opts

submodule:
	cd .. && git submodule init
	cd .. && git submodule update

boruvka: submodule
	$(MAKE) -C boruvka

opts: submodule
	$(MAKE) -C opts

clean:
	$(MAKE) -C boruvka clean
	$(MAKE) -C opts clean

.PHONY: all submodule boruvka opts clean
