OPENBLAS_CFLAGS := $(shell pkg-config --cflags openblas 2>/dev/null)
OPENBLAS_LIBS := $(shell pkg-config --libs openblas 2>/dev/null)

CXX_EXTRA_FLAGS=-I ../external/lazycsv/include -I ../external/eigen $(OPENBLAS_CFLAGS)
LD_EXTRA_OPT=$(OPENBLAS_LIBS)

include ../Makefile.inc

Makefile.hdrdep:
	@touch $@

depends: Makefile.hdrdep
	makedepend -f Makefile.hdrdep *.c*

release : gauss

gauss  : util.r.o Gauss_main.r.o Gauss_solve.r.o
	$(LD) -fopenmp -o $@ $^ $(LD_EXTRA_OPT)

debug   : util.d.o Gauss_main.d.o Gauss_solve.d.o
	$(LD) -fopenmp -o $@ $^ $(LD_EXTRA_OPT)

utest   : Gauss_test.d.o util.d.o Gauss_solve.d.o
	$(LD) -fopenmp -o $@ $^ $(GTEST_LIBS)

run_utest : utest
	./$^

run_itest : release
	echo "Integration tests..."
	bash ./integration_test.sh

all: depends run_utest run_itest

clean :
	-rm -f *.o *.hdrdep.bak
	-rm -f gauss utest debug
	-rm -rf .itest

cleanall : clean
	@echo > Makefile.hdrdep

-include Makefile.hdrdep
