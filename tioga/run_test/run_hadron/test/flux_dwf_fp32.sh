#!/bin/bash
GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/hadron_Nc4/bin/HadronsXmlRun"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

#export MPICH_SMP_SINGLE_COPY_MODE=CMA # Mark (HPE)  suggested to remove this
export MPICH_OFI_NIC_POLICY=GPU

OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 2048 --shm-mpi 1"

PARAMS=" --grid 16.16.16.8 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} "
#flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8
flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP DWFtest2.xml $PARAMS >& log.flux.N1n8


#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
#
# options: Grid/util/Init.cc
