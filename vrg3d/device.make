#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


##########################  vrg3d/vrg3d/device.make  ###########################
#
# Select supported devices.
#
# Devices can be set using the -use option on the configuration file
# or by defining the corresponding environment variable (e.g. USE_GLUT).
#
#  Set these variables for various hardware.
#  You can do this by setting CXXFLAGS (e.g. -DUSE_GLUT).
#
#     USE_ISENSE
#     USE_TRACKD
#     USE_SPACENAV
#     USE_TUIO
#
#     USE_VRPN
#     USE_GLUT
#
# USE_VRPN (DEFAULT):  Compile with support for VRPN devices.
#
# USE_ISENSE (OPTIONAL):  Compile with support for the IS900 tracking device
# by linking directly with Intersense's API as opposed to going through vrpn
# or trackd.
#
# USE_GLUT (OPTIONAL):  Use GLUT rather than G3D's default method for opening
# up a window.  With some gfx card + driver + OS combinations there are bugs
# in correctly opening a stereo window.  Glut seems to almost always work in
# these situations even when other methods fail, so sometimes it is useful to
# add this dependency.  If USE_GLUT is defined all graphics
# windows are automatically created through calls to the glut library.
#
# USE_SPACENAV (OPTIONAL):  Compile with support for 3DCOnnexion's Space Navigator device.
#
# USE_TRACKD (OPTIONAL):  Compile with support for device input from trackd by
# going through the trackdAPI available from VRCO.  Note, VRCO licenses
# trackdAPI for free, you need to contact their support to sign a license,
# then you can download it from them.  trackd is used with cavelib, so it is
# widely supported at various CAVE installations.
#
# USE_TUIO (OPTIONAL):  Compile with support for Tablets.
#...............................................................................


USE_GLUT ?= undefined
ifeq ($(USE_GLUT),undefined)
   USE_GLUT =
else
   USE_GLUT  = 1
   CXXFLAGS += -DUSE_GLUT

   ifeq ($(PLATFORM),linux)
      ifneq ($(IMPORT_GLUT),"/usr")
         LDLIB += -L$(IMPORT_GLUT)
      endif

      LDLIB += -lglut
   endif

   ifeq ($(PLATFORM),OSX)
   endif

   ifeq ($(PLATFORM),WIN32)
      LDLIB += -LIBPATH:"$(IMPORT_GLUT)"
   endif
endif

USE_SPACENAV ?= undefined
ifeq ($(USE_SPACENAV),undefined)
   USE_SPACENAV =
else
   USE_SPACENAV = 1
   CXXFLAGS += -DUSE_SPACENAV

   ifeq ($(PLATFORM),linux)
      LDLIB += -lspnav
   endif
endif


USE_ISENSE ?= undefined
ifeq ($(USE_ISENSE),undefined)
   USE_ISENSE =
else
   USE_ISENSE = 1
   CXXFLAGS += -DUSE_ISENSE
endif


USE_TRACKD ?= undefined
ifeq ($(USE_TRACKD),undefined)
   USE_TRACKD =
else
   USE_TRACKD = 1
   CXXFLAGS += -DUSE_TRACKD

   ifeq ($(PLATFORM),linux)
      LDLIB += -ltrackdAPI
   endif

   ifeq ($(PLATFORM),OSX)
      LDLIB += -ltrackdAPI
   endif

   ifeq ($(PLATFORM),WIN32)
      LDLIB += trackdAPI_MTs.lib
   endif
endif


USE_TUIO ?= undefined
ifeq ($(USE_TUIO),undefined)
   USE_TUIO =
else
   USE_TUIO  = 1
   CXXFLAGS += -DUSE_TUIO

   ifeq ($(PLATFORM),linux)
      LDLIB += -lTUIO
   endif

   ifeq ($(PLATFORM),OSX)
      LDLIB += -lTUIO
   endif

   ifeq ($(PLATFORM),WIN32)
      LDLIB += TUIO.lib
   endif
endif

#
########################  end  vrg3d/vrg3d/device.make  ########################
