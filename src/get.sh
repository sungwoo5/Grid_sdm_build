#!/bin/bash

# wget http://usqcd-software.github.io/downloads/c-lime/lime-1.3.2.tar.gz
# tar xvzf lime-1.3.2.tar.gz


#=========================================================
# git clone https://paboyle@github.com/paboyle/Grid
# cd Grid
# git checkout da593796123f99307b486350f8b2ef6ae7d2c375 # tested version (Mar 26, 2024)
# ./bootstrap.sh
# cd -

# # fix GaugeConfigurationMasked.h for SU(4)
# GRID_SMEARING=Grid/Grid/qcd/smearing
# mv ${GRID_SMEARING}/GaugeConfigurationMasked.h ${GRID_SMEARING}/GaugeConfigurationMasked.h0
# cp -a GaugeConfigurationMasked_fix.h ${GRID_SMEARING}/GaugeConfigurationMasked.h



#=========================================================
git clone git@github.com:aportelli/Hadrons.git
cd Hadrons
git checkout 99d77b8f6a584d67edfe5c88e2238bd00c9b8e15
./bootstrap.sh
cd -

# sungwoo: the following commit makes Hadron incompatible with the Grid version above
#          so we need to use the older version
# commit 3bbcb3dbd22eb6bffb3c811456200fdc1ceea286
# Author: Simon Bürger <simon.buerger@rwth-aachen.de>
# Date:   Fri May 24 00:34:35 2024 +0100
#
#     compatibility with latest Grid


# Add the followings for the wilsonloop measurement
# flow_result[4].data.push_back(WilsonLoops<GImpl>::avgPlaquette(U));
# flow_result[5].data.push_back(WilsonLoops<GImpl>::avgRectangle(U));
# flow_result[6].data.push_back(WilsonLoops<GImpl>::avgPolyakovLoop(U).real());
# flow_result[7].data.push_back(WilsonLoops<GImpl>::avgPolyakovLoop(U).imag());
HADRON_MODULE_MGAUGE=Hadrons/Hadrons/Modules/MGauge
mv ${HADRON_MODULE_MGAUGE}/FlowObservables.hpp ${HADRON_MODULE_MGAUGE}/FlowObservables.hpp0
cp -a FlowObservables.hpp ${HADRON_MODULE_MGAUGE}/.
