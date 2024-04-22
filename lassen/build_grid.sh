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
	 --enable-comms=mpi \
	 --enable-unified=no \
	 --enable-shm=no \
	 --enable-tracing=timer \
	 --enable-accelerator=cuda \
	 --enable-gen-simd-width=32  \
	 --enable-simd=GPU \
         --enable-accelerator-aware-mpi \
	 --enable-openmp \
	 --enable-Nc=${Nc} \
	 --disable-gparity \
	 --disable-fermion-reps \
	 --with-lime=$INSTALLDIR/lime \
	 --prefix=$INSTALLDIR/Grid_omp_Nc${Nc} \
	 CXX=$CXX \
	 CXXFLAGS="${MPI_CFLAGS} -ccbin ${MPICXX} -gencode arch=compute_70,code=sm_70 -Xcompiler -fPIC -Xcompiler -fopenmp" \
	 LDFLAGS="-lcublas -lcudart  ${MPI_LDFLAGS}"

# Examples for Tesla V100 (Summit, Lassen)
# https://github.com/paboyle/Grid/blob/develop/systems/Summit/config-command

# --enable-accelerator-cshift <- deprecated in the recent Grid version
# --enable-accelerator-aware-mpi is default instead

# --enable-shm=nvlink <- ~2.6% slower in Benchmark_dwf_fp32 --mpi 2.2.1.1 --grid 32.32.32.16

make -j 14
make install

popd
