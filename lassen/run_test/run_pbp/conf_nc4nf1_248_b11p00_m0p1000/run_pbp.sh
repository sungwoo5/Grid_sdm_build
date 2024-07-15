#!/bin/bash
### LSF syntax
#BSUB -nnodes 1                   #number of nodes
#BSUB -W 120                      #walltime in minutes
##BSUB -G guests                   #account
#BSUB -e b11.0_m0.1.err             #stderr
#BSUB -o b11.0_m0.1.out             #stdout
#BSUB -J b11.0_m0.1                 #name of job
#BSUB -q pdebug                   #queue to use

source ../../Grid_sdm_build/lassen/env.sh

exe=/usr/WS2/lsd/sungwoo/SU4_sdm/Grid_sdm_build/lassen/build/build_eye2/bin/pbp

cfg_path=../../run_gauge_conf/conf_nc4nf1_248_b11p00_m0p1000/

mkdir -p analysis

#for i in $(seq 4360 10 4574); do 
for i in $(seq 4580 10 4818); do 
    cfg=conf_nc4nf1_248_b11p00_m0p1000_lat.${i}
    lrun -M -gpu -N 1 -n 4 ${exe} $cfg_path $cfg 0.1 --grid 24.24.24.8 --mpi 2.2.1.1 --threads 4 --accelerator-threads 8 --decomposition --debug-mem  --shm 2048 >& log.${i}
done

wait
