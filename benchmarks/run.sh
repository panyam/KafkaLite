#!/bin/sh

rm -rf ../bld ; mkdir -p ../bld ; cd ../bld
gcc -Wall -std=c99 -pg -c ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
gcc -I ../src/ -Wall -std=c99 -pg *.c ../bld/*.o -o ../bld/benchmark 
../bld/benchmark -t ./messages/0.txt

