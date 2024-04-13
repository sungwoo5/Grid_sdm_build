#!/bin/bash
source env.sh

pushd ${BUILDDIR}

HMC=gauge_gen_Nc4
if [ -d ./build_${HMC} ];
then
    rm -rf ./build_${HMC}
fi
if [ -d ../install/${HMC} ];
then
    rm -rf ../install/${HMC}
fi
if [ ! -d ./build_grid_omp_Nc4 ];
then
    echo "Need build_grid_omp_Nc4"
    exit 1
fi

cp -a ../src/${HMC} build_${HMC}
cd ./build_${HMC}

make
make install
popd
