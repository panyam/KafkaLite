#!/bin/sh

rm -rf ../bld ; mkdir -p ../bld ; cd ../bld
gcc -Wall -g -c -std=gnu99 ../src/*.c
ar -cvq libkafkalite.a *.o
cd -
gcc -std=gnu99 -g *.c -I ../src/ ../bld/*.o -o ../bld/kltests -lpthread -lcmocka
# ../bld/kltests
valgrind --leak-check=full -v ../bld/kltests
