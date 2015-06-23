#!/bin/sh

mkdir -p ../bld ; cd ../bld
gcc --pedantic -Wall -std=c99 -g -c ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
gcc main.c -I ../src/ -L../bld -o ../bld/kltests -lcheck -lkafkalite
../bld/kltests
