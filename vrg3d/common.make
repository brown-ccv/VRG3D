#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


##########################  vrg3d/vrg3d/common.make  ###########################
#
#  Define platform specific targets.
#
#...............................................................................


#  Use Unix commands unless overridden.
#
MAKE   ?= make
MKDIR  ?= mkdir
RM     ?= rm
CP     ?= cp
ECHO   ?= echo


# Remove -f when the default RM for some versions of make adds it.
#
ifeq ($(RM),rm -f)
  RM = rm
endif



################################################################################
#
# Platform specific targets with separate include files.
#
# Windows sets OS=Windows_NT and OSTYPE is not set.
#
ifeq ($(OS),Windows_NT)
   OSTYPE :=

else
   # The bash shell may not always set OSTYPE; csh does set it.
   #
   ifeq ($(OSTYPE),)
      OSTYPE := $(shell uname)
   endif
endif
 

# On many Linux platforms uname says:  Linux
#
ifeq ($(OSTYPE),Linux)
   OSTYPE := linux

else
   #  Ubantu and Redhat OSTYPE says: linux-gnu
   #
   ifeq ($(OSTYPE),linux-gnu)
      OSTYPE := linux
   endif
endif

ifeq ($(PLATFORM),linux)
   ifneq ($(OSTYPE),linux)
      $(info  You probably need to run:  python  configure.py  [options])
      $(error ERROR:  PLATFORM=$(PLATFORM)  OSTYPE="$(OSTYPE)" expected "linux")
   endif

   include  $(VRG3D_SOURCE)/vrg3d/linux.make
endif

ifeq ($(PLATFORM),OSX)
   ifneq ($(OSTYPE),darwin)
      ifneq ($(OSTYPE),Darwin)
         $(info  You probably need to run:  python  configure.py  [options])
         $(error ERROR:  PLATFORM=$(PLATFORM)  OSTYPE="$(OSTYPE)" expected "darwin")
      endif 
   endif 

   include  $(VRG3D_SOURCE)/vrg3d/osx.make
endif

ifeq ($(PLATFORM),WIN32)
   # On Windows OS = Windows_NT
   # Otherwise  OS is not set or is unspecified.
   #
   ifneq ($(OS),Windows_NT)
      $(info  You probably need to run:  python  configure.py  [options])
      $(error ERROR:  PLATFORM=$(PLATFORM)  OS="$(OS)" expected "Windows_NT")
   endif 

   include  $(VRG3D_SOURCE)/vrg3d/win32.make
endif



################################################################################
#
#   Clean a program or the vrg3d library.
#
.PHONY:  clean_common
clean_common:
	$(RM) -f  $(BOBJO)  $(BOBJG)
ifdef PROGRAM
	$(RM) -f  obj/$(PROGO)  obj/$(PROGG)   $(CLEAN_JUNK)
endif
ifdef LIBRARY
	$(RM) -f  obj/$(LIBO)  obj/$(LIBG)
endif


# Creat a directory for results.
#
.PHONY:  mkdir_obj
mkdir_obj:
	$(MKDIR)  -p  obj



################################################################################
#
#  Install targets for a program or the vrg3d library.
#  The WORD variable is set to 32 or 64 depending on the OS width.
#  This is an experimental setting and is not yet supported.
#
WORD ?=

ifdef PROGRAM
install_program:
	$(MKDIR) -p  $(INSTALL_VRG3D)/bin$(WORD)/vrg3d
	$(CP) -f  obj/$(PROGO)  $(INSTALL_VRG3D)/bin$(WORD)/vrg3d
	$(CP) -f  obj/$(PROGG)  $(INSTALL_VRG3D)/bin$(WORD)/vrg3d
endif


ifdef LIBRARY
install_library:
	$(MKDIR) -p  $(INSTALL_VRG3D)/include/vrg3d
	$(MKDIR) -p  $(INSTALL_VRG3D)/lib$(WORD)/vrg3d
	$(CP) -f  $(INCLUDE_H)   $(INSTALL_VRG3D)/include/vrg3d
	$(CP) -f  obj/$(LIBO)  $(INSTALL_VRG3D)/lib$(WORD)/vrg3d
	$(CP) -f  obj/$(LIBG)  $(INSTALL_VRG3D)/lib$(WORD)/vrg3d
ifdef LIB_DEBUG
	$(CP) -f  obj/$(LIB_DEBUG)  $(INSTALL_VRG3D)/lib$(WORD)/vrg3d
endif
endif

#
########################  end  vrg3d/vrg3d/common.make  ########################
