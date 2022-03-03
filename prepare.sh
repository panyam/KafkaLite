#! /bin/sh

LIBTOOLIZE=libtoolize
if [ `uname` = "Darwin" ]; then
  LIBTOOLIZE=glibtoolize
fi
mkdir -p pkg/m4 pkg/build-aux
$LIBTOOLIZE && autoheader && aclocal && automake --gnu --add-missing && autoconf
