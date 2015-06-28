
# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -I src
AM_LDFLAGS = -lkafkalite -lpthread -lcppunit

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = kltests
kltests_SOURCES = klcontexttests.cc klcontexttests.h kliteratortests.cc kliteratortests.h kltests.cc kltests.h kltopictests.cc kltopictests.h main.c
