#!/bin/bash
set -x
set -e

cd /home/aaa/fp-solver/cvc5

sudo apt-get install -y flex libfl-dev

./configure.sh --prefix=/home/aaa/fp-solver/cvc5/install --no-java-bindings --no-python-bindings --auto-download

cd build
make -j $(nproc)
sudo make install
