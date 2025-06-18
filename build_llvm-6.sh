#!/bin/bash

# This script builds Z3
set -x
set -e

cd /home/aaa/fp-solver/llvm-6

sudo rm -rf build
mkdir -p build
cd build

cmake \
     -DCMAKE_INSTALL_PREFIX=/home/aaa/fp-solver/llvm-6/install \
     -DCMAKE_BUILD_TYPE=Release \
     -DLLVM_TARGETS_TO_BUILD=X86 \
     -DLLVM_ENABLE_RTTI=OFF \
     -DLLVM_ENABLE_EH=OFF \
     -DLLVM_ENABLE_LTO=OFF \
     -DLLVM_BUILD_LLVM_DYLIB=OFF \
     -DBUILD_SHARED_LIBS=OFF \
     ..

make -j$(nproc)

sudo make install
