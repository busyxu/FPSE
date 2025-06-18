#!/bin/bash

# This script builds Z3
set -x
set -e

cd /home/aaa/fp-solver/gmp-6.2.0x

./configure --enable-cxx

make -j$(nproc)
sudo make install
