#!/bin/sh

rm -rf ../bld ; mkdir -p ../bld ; cd ../bld
gcc -Wall -std=c99 -g -c ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
gcc -g -I ../src/ -Wall -std=c99 *.c ../bld/*.o -o ../bld/benchmark -lpthread
../bld/benchmark -t ./messages/0.txt -l 1000 -c 5
# strace -o trace -ff -ttT ../bld/benchmark -t ./messages/0.txt -l 1000000

