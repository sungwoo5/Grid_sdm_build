#!/bin/bash


if [ $# -ne 4 ]; then
    echo "Usage: $0 <enslabel> <i_traj> <separation> <f_traj>"
    echo "<enslabel>: ex) conf_nc4nf1_248_b12p00c_m0p4000"
    echo "execute this script at Oslic to get the configurations copied"
    
    exit 1
fi

ENSLABEL=$1
CONFSTART=$(printf "%d" $2)
SEPARATION=$(printf "%d" $3)
CONFEND=$(printf "%d" $4)

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

# create rundir
mkdir -p ${RUNDIR}

if [  ${VOL} == "248" ]; then
    LX=24
    LT=8
elif [  ${VOL} == "328" ]; then
    LX=32
    LT=8
fi

# prepare configuration files from lustre
CFGDIR=${RUNDIR}/cfgs
mkdir -p ${CFGDIR}
cp -av base/getcfg.sh ${CFGDIR}/.
cd ${CFGDIR}
./getcfg.sh $CONFSTART $SEPARATION $CONFEND
cd -

# generate xmls
BASE=base/base_batch_n1.sh

BATCH=${RUNDIR}/bsub_$CONFSTART.sh
cp -a $BASE $BATCH
sed -i "s/CONFSTART/"$CONFSTART"/g" $BATCH
sed -i "s/CONFEND/"$CONFEND"/g" $BATCH
sed -i "s/SEPARATION/"$SEPARATION"/g" $BATCH
sed -i "s/LX/"$LX"/g" $BATCH
sed -i "s/LT/"$LT"/g" $BATCH
sed -i "s/RUNDIR/"$LABEL"/g" $BATCH


