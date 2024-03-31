#!/bin/bash

GRID_DIR=/usr/WS2/lsd/sungwoo/SU4_sdm/code/build_tioga_032924_2
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/Grid_Nc4/bin/Benchmark_dwf_fp32"
#APP="$GRID_DIR/install/Grid_Nc4/bin/Benchmark_ITT"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

export MPICH_SMP_SINGLE_COPY_MODE=CMA
export OMP_NUM_THREADS=4
export MPICH_OFI_NIC_POLICY=GPU

PARAMS=" --grid 32.32.32.16 --threads 8  --accelerator-threads 8  --mpi 2.2.2.1 --comms-sequential --shm 2048 --shm-mpi 1"
# PARAMS=" --grid 16.16.16.64 --accelerator-threads 8  --mpi 1.1.2.2 --comms-sequential --shm 2048 --shm-mpi 1"
flux run -N 2 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N2n8

# PARAMS=" --grid 32.32.32.16 --threads 8  --accelerator-threads 8  --mpi 2.2.2.2 --comms-sequential --shm 2048 --shm-mpi 1"
# # PARAMS=" --grid 16.16.16.64 --accelerator-threads 8  --mpi 1.1.2.2 --comms-sequential --shm 2048 --shm-mpi 1"
# flux run -N 2 -n 16 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N2n16

#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
