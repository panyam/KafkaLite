#!/bin/sh

rm -rf ../bld ; mkdir -p ../bld ; cd ../bld
gcc -Wall -std=c99 -g -c ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
gcc -g -I ../src/ -Wall -std=c99 *.c ../bld/*.o -o ../bld/benchmark -lpthread
ARGS="-l 1000 -c 10 -nt 4"
../bld/benchmark $ARGS
# strace -o trace -ff -ttT ../bld/benchmark $ARGS
# valgrind --leak-check=full -v ../bld/benchmark $ARGS

