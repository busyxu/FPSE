#!/bin/bash
set -x
set -e

cd /home/aaa/fp-solver/bitwuzla

export http_proxy=http://192.168.1.3:10809
export https_proxy=http://192.168.1.3:10809


./contrib/setup-cadical.sh
./contrib/setup-btor2tools.sh
./contrib/setup-symfpu.sh

./configure.sh --shared --prefix /home/aaa/fp-solver/bitwuzla/install

cd build
make -j $(nproc)
sudo make install
