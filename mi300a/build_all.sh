#!/bin/bash


mkdir -p build
mkdir -p install

./build_lime.sh >& log.lime

# SU(4)
./build_grid.sh 4 >& log.grid_Nc4
./build_grid_unified.sh 4 >& log.grid_unified_Nc4
./build_gauge_gen.sh >& log.gauge_gen_Nc4

# SU(3)
./build_grid.sh 3 >& log.grid_Nc3
./build_grid_unified.sh 3 >& log.grid_unified_Nc3

