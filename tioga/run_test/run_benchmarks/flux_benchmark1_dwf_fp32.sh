#!/bin/bash

GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/Grid_omp_Nc4/bin/Benchmark_dwf_fp32"
#APP="$GRID_DIR/install/Grid_Nc4/bin/Benchmark_ITT"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

# export MPICH_SMP_SINGLE_COPY_MODE=CMA # Mark (HPE)  suggested to remove this
export MPICH_OFI_NIC_POLICY=GPU

# OPTIONS="--decomposition  --dslash-unroll --comms-concurrent --comms-overlap" # failed
OPTIONS="--decomposition  --comms-concurrent --comms-overlap"


# PARAMS=" --grid 32.32.32.16 --threads 8  --accelerator-threads 8  --mpi 1.2.2.2 --comms-sequential --shm 2048 --shm-mpi 1"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8.comms-seq


PARAMS=" --grid 32.32.32.16 --threads 8  --accelerator-threads 8  --mpi 2.2.2.1 ${OPTIONS}"
flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8

PARAMS=" --grid 32.32.32.16 --threads 8  --accelerator-threads 8  --mpi 2.2.1.1 ${OPTIONS}"
flux run -N 1 -n 4 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n4

PARAMS=" --grid 32.32.32.16 --threads 8  --accelerator-threads 8  --mpi 2.1.1.1 ${OPTIONS}"
flux run -N 1 -n 2 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n2

PARAMS=" --grid 32.32.32.16 --threads 8  --accelerator-threads 8  --mpi 1.1.1.1 ${OPTIONS}"
flux run -N 1 -n 1 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n1

#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU


#-------------------------
# It is --comms-sequential applied by default,
# but  --comms-concurrent --comms-overlap improves the MPI comm
# https://github.com/paboyle/Grid/blob/da593796123f99307b486350f8b2ef6ae7d2c375/Grid/util/Init.cc#L466
