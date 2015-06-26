#!/bin/sh

rm -rf ../bld ; mkdir -p ../bld ; cd ../bld
gcc -Wall -std=c99 -g -c ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
g++ -g *.c -I ../src/ ../bld/*.o -o ../bld/benchmark 
../bld/benchmark
