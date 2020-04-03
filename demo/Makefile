#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


############################  vrg3d/demo/Makefile  #############################
#
#  Compile and install the vrg3d demo.
#...............................................................................
#
#  make             Incremental compile of the optimized demo.
#  make  debug      Incremental compile of the debugging demo.
#  make  all        Incremental compile of the optimized and debugging demo.
#
#  make  clean      Remove intermediate files for the demo.
#  make  allclean   Same as clean.
#
#  make  install    Install build results for the demo.
#...............................................................................


PROGRAM   = vrg3d_demo
CXXSOURCE = vrg3d_demo.cpp


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
############################  vrg3d/demo/Makefile  #############################
