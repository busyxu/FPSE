#!/bin/bash

# This script builds Z3
set -x
set -e

cd /home/aaa/fp-solver/nlopt2

sudo rm -rf build
mkdir -p build
cd build

cmake -DCMAKE_INSTALL_PREFIX=/home/aaa/fp-solver/nlopt2/install ..

make -j$(nproc)
sudo make install


