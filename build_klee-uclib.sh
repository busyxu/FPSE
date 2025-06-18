#!/bin/bash

# This script builds klee-uclib
set -x
set -e

# add by yx
git config --global http.proxy http://192.168.1.3:10809
git config --global https.proxy https://192.168.1.3:10809
git clone https://github.com/klee/klee-uclibc.git

cd klee-uclibc

./configure --make-llvm-lib

make -j$(nproc)
