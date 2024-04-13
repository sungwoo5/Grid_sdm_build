#!/bin/bash
#FLUX: -t 180m
#FLUX: --job-name=6432
#FLUX: --output=6432
#FLUX: --error=6432
#FLUX: -N 2
#FLUX: -n 16
#FLUX: --exclusive


echo "--start " `date` `date +%s`
GRID_DIR=../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/gauge_gen_Nc4/bin/dweofa_mobius_HSDM_v3"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

export MPICH_OFI_NIC_POLICY=GPU

OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 2048 --shm-mpi 1"
PARAMS=" --grid 64.64.64.32 --mpi 2.2.2.2 --threads 8 --accelerator-threads 8 ${OPTIONS} --ParameterFile ip_hmc_mobius_trj2.xml"
flux run -N 2 -n 16 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS


#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
#
# options: Grid/util/Init.cc


#-------------------------------------
# Useful links
# Batch System Cross-Reference Guides: https://hpc.llnl.gov/banks-jobs/running-jobs/batch-system-cross-reference-guides
# Batch jobs: https://hpc-tutorials.llnl.gov/flux/section3/
# Flux cheat sheet: https://flux-framework.org/cheat-sheet/

echo "--end " `date` `date +%s`
