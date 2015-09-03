ACLOCAL_AMFLAGS = -I m4 --install

# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall
AM_CXXFLAGS = --pedantic -Wall

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
TESTS=kltests
check_PROGRAMS = kltests
kltests_SOURCES = klcontexttests.cc klcontexttests.h kliteratortests.cc kliteratortests.h kltests.cc kltests.h kltopictests.cc kltopictests.h main.cc
kltests_LDADD = -lkafkalite -lcmocka
kltests_LDFLAGS = -pthread
kltests_CXXFLAGS = -L$(top_builddir)/src -I$(top_builddir)/src
