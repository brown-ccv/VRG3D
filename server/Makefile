#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


###########################  vrg3d/server/Makefile  ############################
#
#  Compile and install the vrg3d server.
#
#...............................................................................
#
#  make             Incremental compile of the optimized server.
#  make  debug      Incremental compile of the debugging server.
#  make  all        Incremental compile of the optimized and debugging server.
#
#  make  clean      Remove intermediate files for the server.
#  make  allclean   Same as clean.
#
#  make  install    Install build results for the server.
#
#...............................................................................


PROGRAM   = vrg3d_server
CXXSOURCE = vrg3d_server.cpp


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


.PHONY:  install
install:  install_program

#
###########################  vrg3d/server/Makefile  ############################
