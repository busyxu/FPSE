#!/bin/bash
set -x
set -e

cd /home/aaa/fp-solver/klee-float-solver

sudo rm -rf build
mkdir -p build
cd build

cmake -DENABLE_SOLVER_Z3=ON  \
   -DENABLE_SOLVER_STP=OFF \
   -DENABLE_POSIX_RUNTIME=ON \
   -DENABLE_KLEE_UCLIBC=ON \
   -DENABLE_UNIT_TESTS=OFF \
   -DENABLE_SYSTEM_TESTS=OFF \
   -DCMAKE_BUILD_TYPE=Release  ..


make -j $(nproc)
#sudo make install
