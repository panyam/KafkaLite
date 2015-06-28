#!/bin/sh

rm -rf ../bld ; mkdir -p ../bld ; cd ../bld
gcc -Wall -g -c -std=gnu99 ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
g++ -g *.cc main.cc -I ../src/ ../bld/*.o -o ../bld/kltests -lcppunit  -lpthread
../bld/kltests
