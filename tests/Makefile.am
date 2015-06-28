ACLOCAL_AMFLAGS = -I m4 --install

# what flags you want to pass to the C compiler & linker
AM_CXXFLAGS = --pedantic -Wall

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
check_PROGRAMS = kltests
kltests_SOURCES = klcontexttests.cc klcontexttests.h kliteratortests.cc kliteratortests.h kltests.cc kltests.h kltopictests.cc kltopictests.h main.c
kltests_LDADD = -lpthread -lcppunit -lkafkalite
kltests_CFLAGS = -I../src
