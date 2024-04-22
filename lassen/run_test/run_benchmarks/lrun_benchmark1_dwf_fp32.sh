#!/bin/bash

GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/Grid_omp_Nc4/bin/Benchmark_dwf_fp32"

#OPTIONS="--decomposition  --dslash-unroll --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"
OPTIONS="--decomposition --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"

# 1 Nvidia (Lassen) GPU gives a runtime error: https://github.com/paboyle/Grid/issues/452
# PARAMS=" --grid 32.32.32.16 --threads 20  --accelerator-threads 4 --mpi 1.1.1.1 ${OPTIONS}"
# lrun -M -gpu -N 1 -n 1 $APP $PARAMS >& log.lrun.N1n1

PARAMS=" --grid 32.32.32.16 --threads 20  --accelerator-threads 4 --mpi 2.1.1.1 ${OPTIONS}"
lrun -M -gpu -N 1 -n 2 $APP $PARAMS >& log.lrun.N1n2

PARAMS=" --grid 32.32.32.16 --threads 20  --accelerator-threads 4 --mpi 2.2.1.1 ${OPTIONS}"
lrun -M -gpu -N 1 -n 4 $APP $PARAMS >& log.lrun.N1n4

#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 4: how many threads run on each SM in GPU


#-------------------------
# It is --comms-sequential applied by default,
# but  --comms-concurrent --comms-overlap improves the MPI comm
# https://github.com/paboyle/Grid/blob/da593796123f99307b486350f8b2ef6ae7d2c375/Grid/util/Init.cc#L466


# 041824 sungwoo
# --comms-concurrent --comms-overlap makes error
# --accelerator-threads 4 will be good

# 041924 sungwoo
# https://hpc.llnl.gov/documentation/tutorials/using-lc-s-sierra-systems
# -M "-gpu"Turns on CUDA-aware Spectrum MPI
