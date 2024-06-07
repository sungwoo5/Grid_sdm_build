#!/bin/bash
#FLUX: -t 180m
#FLUX: --job-name=p40
#FLUX: --output=p40
#FLUX: --error=p40
#FLUX: -N 1
#FLUX: --exclusive


echo "--start " `date` `date +%s`
# GRID_DIR=../..
GRID_DIR=/usr/WS2/lsd/sungwoo/SU4_sdm/Grid_sdm_build/tioga
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/hadron_Nc4/bin/HadronsXmlRun"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

#export MPICH_SMP_SINGLE_COPY_MODE=CMA # Mark (HPE)  suggested to remove this
export MPICH_OFI_NIC_POLICY=GPU

OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 2048 --shm-mpi 1"

PARAMS=" --grid 16.16.16.8 --mpi 1.1.1.1 --threads 4 --accelerator-threads 8 ${OPTIONS} "
#flux run -N 1 -n 8 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 $APP $PARAMS >& log.flux.N1n8
# ROCR_VISIBLE_DEVICES=0 $APP DWF_0.6.xml $PARAMS >& log.flux.0.6 &
# ROCR_VISIBLE_DEVICES=1 $APP DWF_1.0.xml $PARAMS >& log.flux.1.0 &
# ROCR_VISIBLE_DEVICES=2 $APP DWF_1.4.xml $PARAMS >& log.flux.1.4 &
# ROCR_VISIBLE_DEVICES=3 $APP DWF_1.8.xml $PARAMS >& log.flux.1.8 &
# ROCR_VISIBLE_DEVICES=4 $APP DWF_2.2.xml $PARAMS >& log.flux.2.2 &
ROCR_VISIBLE_DEVICES=3 $APP DWF_1.8.xml $PARAMS >& log.flux.1.8_cont &
ROCR_VISIBLE_DEVICES=4 $APP DWF_2.2.xml $PARAMS >& log.flux.2.2_cont &


wait
#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
#
# options: Grid/util/Init.cc
echo "--end " `date` `date +%s`
