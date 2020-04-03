#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


###########################  vrg3d/example/Makefile  ###########################
#
#  Compile and install the vrg3d VRShaderExample.
#...............................................................................
#
#  make             Incremental compile of the optimized example.
#  make  debug      Incremental compile of the debugging example.
#  make  all        Incremental compile of the optimized and debugging example.
#
#  make  clean      Remove intermediate files for the example.
#  make  allclean   Same as clean.
#
#  make  install    Install build results for the example.
#...............................................................................



PROGRAM   = VRShaderExample
CXXSOURCE = VRShaderExample.cpp


# Configuration parameters and platform specific targets.
#
include  ../vrg3d/configure.make
include  ../vrg3d/common.make

.PHONY: clean
clean:  clean_common

.PHONY: allclean
allclean:  clean

.PHONY: all
all:  default  debug

.PHONY: install
install:  install_program

#
##########################  vrg3d/example/Makefile  ############################
