#!/bin/bash
module purge
module load cuda/11.2.0		# default, also suggested in https://github.com/paboyle/Grid/issues/346
module load gcc/8.3.1
module load cmake/3.23.1
#module load clang/16.0.6-cuda-11.8.0-gcc-11.2.1  # clang makes compilation error

module list

#=====================================
# # https://github.com/paboyle/Grid/blob/develop/systems/Summit/sourceme-cuda10.sh
# export UCX_GDR_COPY_RCACHE=no
# export UCX_MEMTYPE_CACHE=n
# export UCX_RNDV_SCHEME=put_zcopy

export MPI_CFLAGS="-I${CUDA_HOME}/include"
export MPI_LDFLAGS="-L${CUDA_HOME}/lib64"


CC=mpicc
CXX=nvcc
MPICXX=mpicxx

# echo "MPI_CFLAGS=${MPI_CFLAGS}"
# echo "MPI_LDFLAGS=${MPI_LDFLAGS}"
#----------------------------------------------------------
# The directory containing the build scripts, this script and the src/ tree
TOPDIR=`pwd -P`

# Install directory
INSTALLDIR=${TOPDIR}/install

# Source directory
SRCDIR=${TOPDIR}/src

# Build directory
BUILDDIR=${TOPDIR}/build

