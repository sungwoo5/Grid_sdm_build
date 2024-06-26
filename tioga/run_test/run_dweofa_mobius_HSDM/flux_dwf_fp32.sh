#!/bin/bash

GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/gauge_gen_Nc4/bin/dweofa_mobius_HSDM_v3"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

#export MPICH_SMP_SINGLE_COPY_MODE=CMA # Mark (HPE)  suggested to remove this
export MPICH_OFI_NIC_POLICY=GPU

OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 2048 --shm-mpi 1"

# PARAMS=" --grid 16.16.16.8 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius.xml"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8

# PARAMS=" --grid 32.32.32.16 --mpi 2.2.2.2 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius.xml"
# flux run -N 2 -n 16 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N2n16

# PARAMS=" --grid 32.32.32.16 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius_trj2.xml"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8.3216

# OPTIONS="--decomposition  --comms-concurrent --comms-overlap"
# PARAMS=" --grid 32.32.32.16 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius_trj2.xml"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8.3216


# PARAMS=" --grid 64.64.64.16 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius_trj2.xml"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8.6416


# OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 4096 --shm-mpi 1"
# PARAMS=" --grid 64.64.64.32 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius_trj2.xml"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8.6432

# OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 4096 --shm-mpi 1"
# PARAMS=" --grid 64.64.64.24 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius_trj2.xml"
# flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8.6424

OPTIONS="--decomposition  --comms-concurrent --comms-overlap --shm 2048 --shm-mpi 1"

PARAMS=" --grid 64.64.64.16 --mpi 2.2.2.1 --threads 16 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius_trj2.xml"
flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8.6416.acctr16


#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
#
# options: Grid/util/Init.cc
