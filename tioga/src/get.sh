#!/bin/bash

wget http://usqcd-software.github.io/downloads/c-lime/lime-1.3.2.tar.gz
tar xvzf lime-1.3.2.tar.gz



git clone https://paboyle@github.com/paboyle/Grid
cd Grid
git checkout da593796123f99307b486350f8b2ef6ae7d2c375 # tested version (Mar 26, 2024)
./bootstrap.sh
cd -

# fix GaugeConfigurationMasked.h for SU(4)
GRID_SMEARING=Grid/Grid/qcd/smearing
mv ${GRID_SMEARING}/GaugeConfigurationMasked.h ${GRID_SMEARING}/GaugeConfigurationMasked.h0
cp -a GaugeConfigurationMasked_fix.h ${GRID_SMEARING}/GaugeConfigurationMasked.h
