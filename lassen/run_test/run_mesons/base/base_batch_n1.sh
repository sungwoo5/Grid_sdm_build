#!/bin/bash
### LSF syntax
#BSUB -nnodes 1                   #number of nodes
#BSUB -W 120                      #walltime in minutes
##BSUB -G guests                   #account
#BSUB -e mesons_CONFSTART.err             #stderr
#BSUB -o mesons_CONFSTART.out             #stdout
#BSUB -J m_RUNDIR                    #name of job
#BSUB -q pdebug                   #queue to use

BUILD_DIR=/usr/WS2/lsd/sungwoo/SU4_sdm/Grid_sdm_build/lassen/
source ${BUILD_DIR}/env.sh
EXE=${BUILD_DIR}/build/build_eye2/bin/Mobius_mesons_xt

# execute this on the nodes that are accessible to lustres
HERE=`pwd -P`
ENSLABEL=$(echo $HERE | awk -F'/' '{print $(NF)}')
VOL=$(echo ${ENSLABEL}| awk -F"_" '{print $3}')


LABEL=${ENSLABEL#conf_nc4nf1_}
MASSSTR=$(echo ${ENSLABEL}| awk -F"_" '{print $5}')
MASS=$(echo ${MASSSTR#m} | sed 's/p/./g')
M5=1.8

OUTPUT_DIR=./dat
mkdir -p ${OUTPUT_DIR}
PARAMS=" --grid LX.LX.LX.LT --mpi 2.2.1.1 --threads 4 --accelerator-threads 8 ${OPTIONS} "

for i in $(seq CONFSTART SEPARATION CONFEND); do 
    cfg=./cfgs/${ENSLABEL}_lat.${i}
    echo "lrun -M -gpu -N 1 -n 4 ${EXE} ${cfg} ${M5} ${MASS} ${OUTPUT_DIR}/mesons_${LABEL}.${i}.h5 ${PARAMS} >& log.${i}"
    lrun -M -gpu -N 1 -n 4 ${EXE} ${cfg} ${M5} ${MASS} ${OUTPUT_DIR}/mesons_${LABEL}.${i}.h5 ${PARAMS} >& log.${i}

    wait
    rm $cfg
done
