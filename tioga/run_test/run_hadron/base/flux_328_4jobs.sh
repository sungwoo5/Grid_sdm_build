#!/bin/bash
#FLUX: -t 360m
#FLUX: --job-name=test
#FLUX: --output=testINIT
#FLUX: --error=testINIT
#FLUX: -N 1
#FLUX: --exclusive


echo "--start " `date` `date +%s`
GRID_DIR=/usr/WS2/lsd/sungwoo/SU4_sdm/Grid_sdm_build/tioga
source ${GRID_DIR}/env.sh
APP="$GRID_DIR/install/hadron_Nc4/bin/HadronsXmlRun"

# if not used, only one gpu will be used
export MPICH_GPU_SUPPORT_ENABLED=1 
export MPICH_OFI_NIC_POLICY=GPU

OPTIONS="--decomposition  --comms-concurrent --comms-overlap --debug-mem  --shm 2048 --shm-mpi 1"

PARAMS=" --grid 32.32.32.8 --mpi 1.1.1.1 --threads 4 --accelerator-threads 8 ${OPTIONS} "

mkdir -p done output

# input xmls
inputs=()
for j in $(seq INIT 50 $((INIT+150))); do
    echo $j
    inputs+=($(ls *_b1?p??*_m0p????_cfg${j}.xml))
done

echo ${inputs[*]}

# scan all the input xmls
# execute 8 jobs simultaneously
i=0
# for f in $(ls *_b1?p??*_m0p????_cfg*.xml | sort -V); do 
for f in ${inputs[@]}; do 

    echo "ROCR_VISIBLE_DEVICES=${i} $APP ${f} $PARAMS >& log.${f} &"
    ROCR_VISIBLE_DEVICES=${i} $APP ${f} $PARAMS >& log.${f%.xml} &
    # inputs+=($f)
    i=$(($i+2))

    if [ $i == 8 ]; then
	wait
	
	# once all the jobs finished,
	# clean them
	i=0
	mv ${inputs[*]} done/.
		
	inputs=()
    fi

done

# catch the remaining jobs
wait

# once all the remaining jobs finished,
# clean them
mv ${inputs[*]} done/.

#-------------------------------------------------------------------
# https://github.com/paboyle/Grid/issues/323
# --threads 8: specifies thread count <= OMP_NUM_THREADS
# --accelerator-threads 8: how many threads run on each SM in GPU
#
# options: Grid/util/Init.cc
echo "--end " `date` `date +%s`
