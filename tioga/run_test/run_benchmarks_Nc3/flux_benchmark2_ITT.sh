#!/bin/bash

GRID_DIR=../..
source ${GRID_DIR}/env.sh
#APP="$GRID_DIR/install/Grid_omp_Nc4/bin/Benchmark_dwf_fp32"
APP="$GRID_DIR/install/Grid_omp_Nc3/bin/Benchmark_ITT"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

# export MPICH_SMP_SINGLE_COPY_MODE=CMA # Mark (HPE)  suggested to remove this
# export OMP_NUM_THREADS=1
export MPICH_OFI_NIC_POLICY=GPU

Lattice="16.16.16.64"
MpiGrid="1.2.2.2"

# PARAMS=" --grid ${Lattice} --mpi ${MpiGrid}  --threads 16  --accelerator-threads 8  --comms-sequential --shm 2048 --shm-mpi 1"
# flux run -N 1 -n 4 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.ITT

# PARAMS=" --grid ${Lattice} --mpi ${MpiGrid}  --threads 16  --accelerator-threads 8  --comms-sequential --shm 2048 --shm-mpi 1"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.ITT.n8


# PARAMS=" --grid ${Lattice} --mpi ${MpiGrid}  --threads 16  --accelerator-threads 8  -decomposition  --dslash-unroll --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"
# flux run -N 1 -n 4 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.ITT.oliver



# MpiGrid="1.1.1.2"
# PARAMS=" --grid ${Lattice} --mpi ${MpiGrid}  --threads 16  --accelerator-threads 8  -decomposition  --dslash-unroll --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"
# flux run -N 1 -n 2 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.ITT.n2

# MpiGrid="1.1.1.1"
# PARAMS=" --grid ${Lattice} --mpi ${MpiGrid}  --threads 16  --accelerator-threads 8  -decomposition  --dslash-unroll --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"
# flux run -N 1 -n 1 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.ITT.n1

MpiGrid="2.2.2.2"
PARAMS=" --grid ${Lattice} --mpi ${MpiGrid}  --threads 16  --accelerator-threads 8  -decomposition  --dslash-unroll --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"
flux run -N 2 -n 16 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.ITT.N2n16


#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
