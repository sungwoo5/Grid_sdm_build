#!/bin/bash
source env.sh
# sites: Lime, MPFR place in grid_prefix
##################
#export prefIx=$GRID_DIR/grid_prefix

pushd ${BUILDDIR}

if [ -d ./build_lime ];
then
  rm -rf ./build_lime
fi

mkdir  ./build_lime
cd ./build_lime




##################
#LIME
##################
${SRCDIR}/lime-1.3.2/configure \
	 --prefix=${INSTALLDIR}/lime \
	 CC=$CC

make
make install

popd
