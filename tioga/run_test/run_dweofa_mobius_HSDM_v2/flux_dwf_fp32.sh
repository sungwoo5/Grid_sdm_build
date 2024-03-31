#!/bin/bash

GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/gauge_gen_Nc4/dweofa_mobius_HSDM_v3"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

export OMP_NUM_THREADS=8


PARAMS=" --grid 16.16.16.8 --mpi 2.2.2.1 --threads 8 --accelerator-threads 8 --ParameterFile ip_hmc_mobius.xml"
flux run -N 2 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N2n8

#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
