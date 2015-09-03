#! /bin/sh

mkdir -p /tmp/libkafkalite
aclocal && automake --gnu --add-missing && autoconf
