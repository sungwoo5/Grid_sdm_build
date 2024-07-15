#!/bin/bash


# Check if the correct number of arguments was provided
# if [ $# -ne 5 ]; then
#     echo "Usage: $0 <NLNT> <beta_str> <mass> <i_traj> <f_traj>"
#     echo "<vol>: ex) 248"
#     echo "<beta_str>: ex) b12p00c"
#     echo "<mass>: ex) 0.4"
    
#     exit 1
# fi
if [ $# -ne 3 ]; then
    echo "Usage: $0 <enslabel> <i_traj> <f_traj>"
    echo "<enslabel>: ex) conf_nc4nf1_248_b12p00c_m0p4000"
    
    exit 1
fi

ENSLABEL=$1
i_traj=$(printf "%d" $2)
f_traj=$(printf "%d" $3)

# Input params from ENSLABEL

VOL=$(echo ${ENSLABEL}| awk -F"_" '{print $3}')
BETASTR=$(echo ${ENSLABEL}| awk -F"_" '{print $4}')
MASSSTR=$(echo ${ENSLABEL}| awk -F"_" '{print $5}')
MASS=$(echo ${MASSSTR#m} | sed 's/p/./g')

# string labels
MASSSTR=$(printf "m%.4f" $MASS | sed 's/\./p/g')
LABEL=${ENSLABEL#conf_nc4nf1_}
RUNDIR=${ENSLABEL}
CFG_PREFIX=${RUNDIR}_lat

# create dir
mkdir -p ${RUNDIR}

# create link to the gauge configuration
if [  ${VOL} == "248" ]; then
    CFGPATH="/p/lustre1/park49/SU4_sdm/run_gauge_conf/conf_nc4nf1_${LABEL}"

    # copy flux.sh
    cp -a base/flux_248.sh ${RUNDIR}/flux.sh

elif [  ${VOL} == "328" ]; then
    CFGPATH="/p/lustre2/park49/SU4_sdm/run_gauge_conf/conf_nc4nf1_${LABEL}"

    # copy flux.sh
    cp -a base/flux_328.sh ${RUNDIR}/flux.sh

fi
if [ -d "$CFGPATH" ]; then
    rm -f ${RUNDIR}/cfgs
    ln -s ${CFGPATH} ${RUNDIR}/cfgs
else
    echo "CFGPATH=${CFGPATH} does not exist"
    exit 1
fi

# generate xmls
BASE=base/base.xml

for CONFSTART in $(seq ${i_traj} 50 $((${f_traj}-1))); do 

    XML=${RUNDIR}/${LABEL}_cfg${CONFSTART}.xml
    CONFEND=$(echo "${CONFSTART}+50"| bc )
    cp -a $BASE $XML
    sed -i "s/CONFSTART/"$CONFSTART"/g" $XML
    sed -i "s/CONFEND/"$CONFEND"/g" $XML
    sed -i "s/MASS/"$MASS"/g" $XML
    sed -i "s/LABEL/"$LABEL"/g" $XML
    sed -i "s/CFG_PREFIX/"$CFG_PREFIX"/g" $XML

done

