#! /bin/sh

mkdir -p pkg/m4 pkg/build-aux
libtoolize && autoheader && aclocal && automake --gnu --add-missing && autoconf
