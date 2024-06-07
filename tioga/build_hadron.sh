#!/bin/bash
source env.sh

if [ "$#" -ne 1 ]; then
    echo "Error: This script requires exactly one argument: Nc (3 or 4)" >&2
    exit 1
fi

Nc=$1

pushd ${SRCDIR}/hadron
autoreconf
popd

pushd ${BUILDDIR}

if [ -d ./build_hadron_Nc${Nc} ];
then
  rm -rf ./build_hadron_Nc${Nc}
fi

mkdir  ./build_hadron_Nc${Nc}
cd ./build_hadron_Nc${Nc}


##################
#HADRON
##################
${SRCDIR}/Hadrons/configure \
	 --prefix=$INSTALLDIR/hadron_Nc${Nc} \
	 --with-grid=$INSTALLDIR/Grid_omp_Nc${Nc} \

# Examples for MI250X
# https://github.com/paboyle/Grid/blob/develop/systems/Frontier/config-command
# https://github.com/paboyle/Grid/blob/develop/systems/Crusher/config-command


make -j 14
make install

popd
