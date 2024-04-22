#!/bin/bash

GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/Grid_omp_Nc3/bin/Benchmark_ITT"

OPTIONS="--threads 20  --accelerator-threads 4  --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"
Lattice="16.16.16.64"
#MpiGrid="1.2.2.2"

#-------------------------------------------------
#https://github.com/paboyle/Grid/issues/452

# MpiGrid="1.1.1.1"

# PARAMS=" --grid ${Lattice} --mpi ${MpiGrid}  --threads 16  --accelerator-threads 8  --comms-sequential --shm 2048 --shm-mpi 1"
# $APP $PARAMS

#-------------------------------------------------
MpiGrid="2.2.1.1"
PARAMS=" --grid ${Lattice} --mpi ${MpiGrid} ${OPTIONS}"
lrun -M -gpu -N 1 -n 4 $APP $PARAMS >& log.ITT.N1n4

#-------------------------------------------------
MpiGrid="2.1.1.1"
PARAMS=" --grid ${Lattice} --mpi ${MpiGrid} ${OPTIONS}"
lrun -M -gpu -N 1 -n 2 $APP $PARAMS >& log.ITT.N1n2


#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
