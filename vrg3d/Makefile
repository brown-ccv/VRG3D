#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


###########################  vrg3d/vrg3d/Makefile  #############################
#
#  G3D 8 only builds on linux with gcc4 and on windows with cl9.
#

# The source code currently uses the following additional variables:
#
#     G3D_OSX        G3D_WIN32      G3D_LINUX     :Set in G3D/plaftorm.h
#     __cplusplus                                 :Set by compiler.
#...............................................................................
#
#  make             Incremental compile of the optimized library.
#  make  debug      Incremental compile of the debugging library.
#  make  all        Incremental compile of the libraries, server and demo.
#
#  make  clean      Remove intermediate files for the libraries.
#  make  allclean   Clean files for the libraries, server, and demo.
#
#  make  install    Install build results for the libraries, server, and demo.
#...............................................................................


# Configuration parameters.
#
include  configure.make

LIBRARY = vrg3d

CXXSOURCE = VRApp.cpp                     InputDevice.cpp            \
            CheckerboardStencil.cpp       ConfigMap.cpp              \
            DisplayTile.cpp               Event.cpp                  \
            G3DOperators.cpp              ICubeXDevice.cpp           \
            Midi.cpp                      MouseToTracker.cpp         \
            ProjectionVRCamera.cpp        SynchedSystem.cpp          \
            HorizontalStencil.cpp


# Device specific varients:
#
ifdef USE_GLUT
   CXXSOURCE += GlutWindow.cpp
   INCLUDE_H += GlutWindow.h
endif

ifdef USE_SPACENAV
   INCLUDE_H += SpaceNavDevice.h
endif

ifdef USE_ISENSE
   CXXSOURCE += ISenseDirect.cpp
   INCLUDE_H += ISenseDirect.h
endif

ifdef USE_TRACKD
   CXXSOURCE += TrackDClient.cpp
   INCLUDE_H += TrackDClient.h
endif

ifdef USE_TUIO
   CXXSOURCE += TUIOEvents.cpp
   INCLUDE_H += TUIOEvents.h
endif



# Platform specific varients:
#
ifeq ($(PLATFORM),OSX)
    CXXSOURCE += VRApp_osx.cpp

    ifdef USE_SPACENAV
       CXXSOURCE += SpaceNavDevice_osx.cpp
    endif

    CXXSOURCE += Midi_linux.cpp
endif
ifeq ($(PLATFORM),linux)
    CXXSOURCE += Midi_linux.cpp  VRApp_linux.cpp

    ifdef USE_SPACENAV
       CXXSOURCE += SpaceNavDevice_linux.cpp
    endif
endif
ifeq ($(PLATFORM),WIN32)
    CXXINC += -I"$(IMPORT_GLUT)"

    CXXSOURCE += Midi_win32.cpp  VRApp_win32.cpp

    ifdef USE_SPACENAV
       CXXSOURCE += SpaceNavDevice_win32.cpp
    endif
endif



# Also compile these files when building with vrpn.
#
CXXSOURCE += VRPNAnalogDevice.cpp      VRPNButtonDevice.cpp          \
             VRPNTrackerDevice.cpp


CXXSOURCE += VRApp_network.cpp


#  Include files to be installed.
#
INCLUDE_H += CheckerboardStencil.h       ConfigMap.h                \
             DisplayTile.h               Event.h                    \
             EventNet.h                  G3DOperators.h             \
             ICubeXDevice.h              InputDevice.h              \
             Midi.h                      MouseToTracker.h           \
                                         ProjectionVRCamera.h       \
             SynchedSystem.h             VRApp.h                    \
             VRG3D.h                     VRPNAnalogDevice.h         \
             VRPNButtonDevice.h          VRPNTrackerDevice.h        \
             documentation.h             HorizontalStencil.h



# Platform specific targets.
#
include  common.make


.PHONY:  all
all:  default  debug
	$(MAKE)  -C ../server    all
	$(MAKE)  -C ../demo      all
	$(MAKE)  -C ../example   all


.PHONY:  clean
clean:  clean_common


.PHONY:  allclean
allclean:  clean
	$(MAKE)  -C ../server    clean
	$(MAKE)  -C ../demo      clean
	$(MAKE)  -C ../example   clean


.PHONY:  install
install:  install_library
	$(MAKE)  -C ../share     install
	$(MAKE)  -C ../server    install
	$(MAKE)  -C ../demo      install
	$(MAKE)  -C ../example   install


#
###########################  vrg3d/vrg3d/Makefile  #############################
