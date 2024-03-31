#!/bin/bash
source env.sh

if [ "$#" -ne 1 ]; then
    echo "Error: This script requires exactly one argument: Nc (3 or 4)" >&2
    exit 1
fi

Nc=$1

pushd ${SRCDIR}/Grid
autoreconf
popd

pushd ${BUILDDIR}

if [ -d ./build_grid_omp_Nc${Nc} ];
then
  rm -rf ./build_grid_omp_Nc${Nc}
fi

mkdir  ./build_grid_omp_Nc${Nc}
cd ./build_grid_omp_Nc${Nc}


##################
#GRID
##################
${SRCDIR}/Grid/configure \
	 --enable-comms=mpi-auto \
	 --enable-unified=no \
	 --enable-shm=nvlink \
	 --enable-tracing=timer \
	 --enable-accelerator=hip \
	 --enable-gen-simd-width=64  \
	 --enable-simd=GPU \
	 --enable-openmp \
	 --enable-accelerator-cshift \
	 --enable-Nc=${Nc} \
	 --disable-gparity \
	 --disable-fermion-reps \
	 --with-lime=$INSTALLDIR/lime \
	 --prefix=$INSTALLDIR/Grid_omp_Nc${Nc} \
	 CXX=$CC \
	 MPICXX=$CC \
	 CXXFLAGS="-fPIC ${MPI_CFLAGS}  -L/lib64  -fgpu-sanitize --offload-arch=gfx90a -fopenmp" \
	 LDFLAGS="-L/lib64 -lamdhip64 -lhipblas -lrocblas ${MPI_LDFLAGS}"

# Examples for MI250X
# https://github.com/paboyle/Grid/blob/develop/systems/Frontier/config-command
# https://github.com/paboyle/Grid/blob/develop/systems/Crusher/config-command


make -j 14
make install

popd
