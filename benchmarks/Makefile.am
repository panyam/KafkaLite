ACLOCAL_AMFLAGS = -I pkg/m4 --install

# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99
AM_CXXFLAGS = --pedantic -Wall

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = klbench
klbench_SOURCES = benchmark.c benchmark.h multi.c single.c utils.c
klbench_LDADD = -lkafkalite
klbench_LDFLAGS = -pthread
klbench_CFLAGS = -L$(top_builddir)/src -I$(top_builddir)/src --pedantic -Wall -std=c99
