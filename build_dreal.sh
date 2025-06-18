#!/bin/bash

# This script builds Z3
set -x
set -e

cd /home/aaa/fp-solver/dreal_install
sudo ./install_dreal.sh

sudo ln -s /opt/dreal/4.21.06.2/lib/libdreal.so /usr/lib/libdreal.so
sudo ln -s /opt/libibex/2.7.4/lib/libibex.so /usr/lib/libibex.so
