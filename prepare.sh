#! /bin/sh

mkdir -p pkg/m4 pkg/build-aux
libtoolize && aclocal && automake --gnu --add-missing && autoconf
