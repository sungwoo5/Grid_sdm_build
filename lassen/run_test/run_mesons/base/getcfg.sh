#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage: $0 <i_traj> <sep_traj> <f_traj>"
    
    exit 1
fi

i_traj=$(printf "%d" $1)
sep_traj=$(printf "%d" $2)
f_traj=$(printf "%d" $3)

# execute this on the nodes that are accessible to lustres
HERE=`pwd -P`
LABEL=$(echo $HERE | awk -F'/' '{print $(NF-1)}')
VOL=$(echo ${LABEL}| awk -F"_" '{print $3}')

LUSTREPATH=""
if [ ${VOL} == "248" ]; then
    LUSTREPATH="/p/lustre1/park49/SU4_sdm/run_gauge_conf/${LABEL}"
    
elif [ ${VOL} == "328" ]; then
    LUSTREPATH="/p/lustre2/park49/SU4_sdm/run_gauge_conf/${LABEL}"
    
fi

# copy start
for j in $(seq ${i_traj} ${sep_traj} ${f_traj} ); do
    rsync -av ${LUSTREPATH}/${LABEL}_lat.${j} .
done
