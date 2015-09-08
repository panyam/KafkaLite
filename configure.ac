#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libkafkalite], [0.0.6], [sri.panyam@gmail.com])
# AC_CONFIG_SRCDIR([benchmarks/benchmark.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([pkg/build-aux])
# AC_CONFIG_LIBOBJ_DIR([portable])
AC_CONFIG_MACRO_DIR([pkg/m4])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CXX_C_O
#AC_PROG_LIBTOOL
# AM_PROG_AR
#AM_PROG_LIBTOOL

LT_INIT
# LT_PREREQ([2.4.1])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h netinet/in.h stdlib.h string.h strings.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday mkdir strdup strerror strndup])

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile benchmarks/Makefile])
AC_OUTPUT
