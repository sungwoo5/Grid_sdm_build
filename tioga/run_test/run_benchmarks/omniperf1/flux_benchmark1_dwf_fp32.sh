#!/bin/bash
GRID_DIR=../../..
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/Grid_omp_Nc4/bin/Benchmark_dwf_fp32"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 

#export MPICH_SMP_SINGLE_COPY_MODE=CMA <- default is XPMEM, will be good enough
export MPICH_OFI_NIC_POLICY=GPU

#module load omniperf
#OMNIPERF="omniperf profile -n peak_profile --roof-only -- "
# this omniperf is nothing but executing the rocprof which is already installed within the rocm module

# rocprof 
# OMNIPERF="rocprof --stats"
# OPTIONS="--decomposition  --comms-concurrent --comms-overlap --threads 8  --accelerator-threads 8"

# # GEOM=" --grid 16.16.16.8 --mpi 1.1.1.1"
# # flux run -N 1 -n 1 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 ${OMNIPERF} $APP ${GEOM} ${OPTIONS} >& log.flux.16.8

# OPTIONS="--decomposition  --comms-concurrent --comms-overlap --threads 8  --accelerator-threads 16"

# GEOM=" --grid 16.16.16.8 --mpi 1.1.1.1"
# flux run -N 1 -n 1 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 ${OMNIPERF} $APP ${GEOM} ${OPTIONS} >& log.flux.16.8.acctr16



module load omniperf
OMNIPERF="omniperf profile -n peak_profile_dev0 --roof-only --sort kernels --device 0 --kernel-names -- "
OPTIONS="--decomposition  --comms-concurrent --comms-overlap --threads 8  --accelerator-threads 8"

# GEOM=" --grid 16.16.16.8 --mpi 1.1.1.1"
# # flux run -N 1 -n 1 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 ${OMNIPERF} $APP ${GEOM} ${OPTIONS} >& log.flux.16.8.omni


# OMNIPERF="omniperf profile -n test1 --device 0 -- " # for the full omniperf
# OPTIONS="--decomposition  --comms-concurrent --comms-overlap --threads 8  --accelerator-threads 8"
# flux run -N 1 -n 1 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 ${OMNIPERF} $APP ${GEOM} ${OPTIONS} >& log.flux.16.8.omnifull


GEOM=" --grid 32.32.32.16 --mpi 1.1.1.1"
OMNIPERF="omniperf profile -n test2 --device 0 -- " # for the full omniperf
OPTIONS="--decomposition  --comms-concurrent --comms-overlap --threads 8  --accelerator-threads 8"
flux run -N 1 -n 1 -g 1 --verbose --exclusive --setopt=mpibind=verbose:1 ${OMNIPERF} $APP ${GEOM} ${OPTIONS} >& log.flux.32.16.omnifull



# omniperf analyze -p workloads/test1/MI200 --list-stats | grep Wilson | head  # analyze
# omniperf analyze -p workloads/test1/MI200 -k 0 -o opout_k0.txt

