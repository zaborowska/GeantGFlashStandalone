#!/bin/sh -u
export FCCEDM=/afs/cern.ch/exp/fcc/sw/0.6/fcc-edm/0.2/x86_64-slc6-gcc49-opt
export PODIO=/afs/cern.ch/exp/fcc/sw/0.6/podio/0.2/x86_64-slc6-gcc49-opt
export CMAKE_PREFIX_PATH=$FCCEDM:$PODIO:$CMAKE_PREFIX_PATH
export PYTHONDIR=/afs/cern.ch/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc48-opt
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PYTHONDIR/lib:$FCCEDM/lib:$PODIO/lib
export PYTHONPATH=$PYTHONPATH:$PODIO/lib:$PODIO/python:$FCCEDM/lib
export BOOST_ROOT=/afs/cern.ch/sw/lcg/external/Boost/1.55.0_python2.7/x86_64-slc6-gcc47-opt/

source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh

# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.0/setup_g4datasets.sh
source /afs/cern.ch/sw/lcg/external/geant4/10.0.p04/x86_64-slc6-gcc49-opt/CMake-setup.sh
source /afs/cern.ch/exp/fcc/sw/0.6/LCG_80/ROOT/6.04.06/x86_64-slc6-gcc49-opt/bin/thisroot.sh
