#!/bin/bash
#SBATCH -p cpu # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 8 # number of cores
#SBATCH -x n33 # exclude node n33
#SBATCH --mem 1G # memory pool for all cores
#SBATCH -t 0-2:00 # time (D-HH:MM)
#SBATCH -o build.%N.%j.out # STDOUT
#SBATCH -e build.%N.%j.err # STDERR

# This script builds the project on the RCI cluster.
# Submit this task from the top directory of this repository as follows:
# sbatch scripts/build-rci.sh

set -x

NCPUS=8

cat >Makefile.local <<EOF
CFLAGS = -march=native
EOF

make mrproper
make -j$NCPUS boruvka
make -j$NCPUS opts
make -j$NCPUS
make -j$NCPUS -C bin
