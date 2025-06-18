#!/bin/bash
set -x
set -e

cd /home/aaa/fp-solver/gosat

sudo rm -rf build
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..

make -j $(nproc)
