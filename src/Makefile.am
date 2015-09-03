ACLOCAL_AMFLAGS = --install
# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99
AM_LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libkafkalite.la
libkafkalite_la_SOURCES = klarray.c klbuffer.c klcontext.c kliterator.c klpthreadmutex.c kltopics.c klutils.c
include_HEADERS = kafkalite.h 
pkginclude_HEADERS = klarray.h klbuffer.h klconcurrency.h klconsumer.h klcontext.h klfwddefs.h kliterator.h klmessage.h klprivate.h klpthreadmutex.h kltopics.h klutils.h
