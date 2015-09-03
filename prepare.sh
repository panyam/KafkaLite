#! /bin/sh

mkdir -p /tmp/libkafkalite/m4
mkdir -p /tmp/libkafkalite/build-aux
aclocal && automake --gnu --add-missing && autoconf
