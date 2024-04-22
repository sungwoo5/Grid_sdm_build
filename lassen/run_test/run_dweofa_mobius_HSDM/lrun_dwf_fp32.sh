#!/bin/bash
GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/gauge_gen_Nc4/bin/dweofa_mobius_HSDM_v3"

OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 2048 --shm-mpi 1"

# PARAMS=" --grid 16.16.16.8 --mpi 2.2.1.1 --threads 20 --accelerator-threads 4 ${OPTIONS} --ParameterFile ip_hmc_mobius.xml"
# lrun -M -gpu -n 4 $APP $PARAMS >& log.lrun.N1n4

PARAMS=" --grid 32.32.32.16 --mpi 2.2.1.1 --threads 20 --accelerator-threads 4 ${OPTIONS} --ParameterFile ip_hmc_mobius.xml"
lrun -M -gpu -n 4 $APP $PARAMS >& log.lrun.N1n4.3216

PARAMS=" --grid 64.64.64.16 --mpi 2.2.1.1 --threads 20 --accelerator-threads 4 ${OPTIONS} --ParameterFile ip_hmc_mobius.xml"
lrun -M -gpu -n 4 $APP $PARAMS >& log.lrun.N1n4.6416

#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
#
# options: Grid/util/Init.cc
