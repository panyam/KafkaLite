#!/bin/sh

rm -rf ../bld ; mkdir -p ../bld ; cd ../bld
gcc -Wall -std=c99 -g -c ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
g++ *.cc main.c -I ../src/ ../bld/*.o -o ../bld/kltests -lcppunit 
../bld/kltests
