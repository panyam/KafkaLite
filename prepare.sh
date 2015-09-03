#! /bin/sh

mkdir -p pkg/m4 pkg/build-aux
aclocal && automake --gnu --add-missing && autoconf
