#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


#########################  common/vrg3d/win32.make  ############################
#
#  Windows Make targets for Vrg3d.
#
#...............................................................................



# Flags correspond to g3d builds.
#
# -Zi      Generates a pdb for each object file.
# -Z7      Generates debugging information inside each object file.
# -DEBUG   Generates a pdb for a program.
# -O2      Creates fast code.
#
# The -MT flag was not needed for the demo???
#
RELEASE_FLAGS += -MT   -O2
DEBUG_FLAGS   += -ZI   -MTd  -Od

ifdef  PROGRAM
    #LDFLAGS_G += -DEBUG  -nodefaultlib:LIBCMT.lib
    LDFLAGS_G += -DEBUG

else
    DEBUG_FLAGS   += -Fdobj/$(LIBRARY)d.pdb
endif


#  -RTCc  Reports when a value is assigned to a smaller data type
#         and results in a data loss.
#
#  -RTCs  Enables stack frame run-time error checking.
#
#  -RTCu  Reports when a variable is used without having been initialized.
#
#  -RTC1  RTCs and RTCu
#
DEBUG_FLAGS += -RTC1


#  Environment settings for visual studio 2008 version 9
#
export  MSDevDir           =

export  VCINSTALLDIR       = $(ProgramFiles)/Microsoft Visual Studio 9.0
export  MSVCDir            = $(VCINSTALLDIR)/VC
export  MSVC9Dir           = $(VCINSTALLDIR)/VC


# Not sure we need these.  VCVars exports them.
#
export  VS90COMNTOOLS      = "$(VCINSTALLDIR)/Common7/Tools"
export  VSINSTALLDIR       = "$(VCINSTALLDIR)/Common7/IDE"
export  DevEnvDir          = $(VSINSTALLDIR)
export  CommonProgramFiles = "$(ProgramFiles)/Common Files"

export  FrameworkDir       = Framework32
export  FrameworkVersion   = v2.0.50727
export  FrameworkSDKDir    = "$(VCINSTALLDIR)/SDK/v3.5"
export  Framework35Version = v3.5



# Explicitly list include paths.
#
CLINC := -I"$(VCINSTALLDIR)\VC\ATLMFC\INCLUDE"
CLINC += -I"$(VCINSTALLDIR)\VC\Include"
CLINC += -I"$(VCINSTALLDIR)\VC\MFC\Include"



# Explicitly list load paths.
#
LDLIB  += -MANIFEST
LDLIB  += -LIBPATH:"$(VCINSTALLDIR)/VC/lib"
LDLIB  += -LIBPATH:"$(VCINSTALLDIR)/VC/MFC/lib"
LDLIB  += -LIBPATH:"$(VCINSTALLDIR)/VC/atlmfc/lib"


ifdef COMPILER
    CC  = "$(subst \,/,$(COMPILER))"
    CXX = "$(subst \,/,$(COMPILER))"

else
    CC  = "$(VCINSTALLDIR)/VC/bin/cl.exe"
    CXX = "$(VCINSTALLDIR)/VC/bin/cl.exe"
endif

LD = "$(VCINSTALLDIR)/VC/bin/link.exe"
AR = "$(VCINSTALLDIR)/VC/bin/lib.exe"


# Find the most recent SDK.
#
SDK_DIRECTORY = $(wildcard $(SystemDrive)/Program*Files/Microsoft*SDKs/Windows/v7.1)

ifeq ($(SDK_DIRECTORY),)
    SDK_DIRECTORY = $(wildcard $(SystemDrive)/Program*Files/Microsoft*SDKs/Windows/v7.0)

    ifeq ($(SDK_DIRECTORY),)
        SDK_DIRECTORY = $(wildcard $(SystemDrive)/Program*Files/Microsoft*SDKs/Windows/v6.1)

        ifeq ($(SDK_DIRECTORY),)
            $(error  ERROR:  Can not locate a Microsoft SDK directory.)
        endif
    endif

    SDK_DIRECTORY_A = $(wildcard $(SystemDrive)/Program*Files/Microsoft*SDKs/Windows/v7.0A)

    ifeq ($(SDK_DIRECTORY_A),)
        SDK_DIRECTORY_A = $(wildcard $(SystemDrive)/Program*Files/Microsoft*SDKs/Windows/v6.0A)

        ifeq ($(SDK_DIRECTORY_A),)
            # $(info  NOTE:  Can not locate a Microsoft SDK version A directory.)
        endif
    endif
endif

# $(info SDK_DIRECTORY=$(SDK_DIRECTORY))
# $(info SDK_DIRECTORY_A=$(SDK_DIRECTORY_A))


# Export the path so that cl can find components.
#
WPATH:=$(SDK_DIRECTORY)\bin
WPATH+=$(VCINSTALLDIR)\VC\vcpackages;$(WPATH)
WPATH+=$(VCINSTALLDIR)\common7\tools\bin;$(WPATH)
WPATH+=$(VCINSTALLDIR)\Common7\Tools;$(WPATH)
WPATH+=$(VCINSTALLDIR)\VC\bin;$(WPATH)
WPATH+=$(VCINSTALLDIR)\Common7\IDE
export  PATH:=$(WPATH);$(PATH)


MT    := "$(SDK_DIRECTORY)/bin/mt.exe"
LDLIB += -LIBPATH:"$(SDK_DIRECTORY)/lib"

CLINC := -I"$(SDK_DIRECTORY)\include"

ifdef SDK_DIRECTORY_A
    CLINC += -I"$(SDK_DIRECTORY_A)\include"
endif

CLINC += -I"$(VCINSTALLDIR)\VC\ATLMFC\INCLUDE"
CLINC += -I"$(VCINSTALLDIR)\VC\Include"
CLINC += -I"$(VCINSTALLDIR)\VC\MFC\Include"

CINC   += $(CLINC)
CXXINC += $(CLINC)


ifdef PROGRAM
PROGO := $(PROGRAM).exe
PROGG := $(PROGRAM)d.exe

default:  obj/$(PROGO)

debug:    obj/$(PROGG)
endif


ifdef LIBRARY
LIBO := $(LIBRARY).lib
LIBG := $(LIBRARY)d.lib
LIB_DEBUG := $(LIBRARY)d.pdb

default:  obj/$(LIBO)

debug:    obj/$(LIBG)
endif


OBJ   := $(CXXSOURCE:.cpp=.o)
OBJO  := $(OBJ:.o=.oo)
OBJG  := $(OBJ:.o=.og)

BOBJO := $(OBJ:%.o=obj/%.oo)
BOBJG := $(OBJ:%.o=obj/%.og)


# Files to always be cleaned.
#
CLEAN_JUNK = obj/$(PROGRAM:%=%.exe.manifest)        \
             obj/$(PROGRAM:%=%.pdb)                 \
             obj/$(PROGRAM:%=%d.exe.manifest)       \
             obj/$(PROGRAM:%=%d.pdb)



# Compile a file.
#
obj/%.og: %.cpp
	$(CXX) $< -c  -Fo"obj/$*.og"  $(CXXFLAGS)  \
          $(CXXINC)                            \
          $(DEBUG_FLAGS)



obj/%.oo: %.cpp
	$(CXX) $< -c  -Fo"obj/$*.oo"  $(CXXFLAGS)  \
          $(CXXINC)                            \
          $(RELEASE_FLAGS)



# Create a library.
#
ifdef LIBO
obj/$(LIBO):  mkdir_obj  $(BOBJO)
	@$(ECHO)  "   <<< Archive:  obj/$(LIBO) >>>"
	cd  obj  &&  $(AR) /verbose /out:$(LIBO) $(OBJO)
endif

ifdef LIBG
obj/$(LIBG):  mkdir_obj  $(BOBJG)
	@$(ECHO)  "   <<< Archive:  obj/$(LIBG) >>>"
	cd  obj  &&  $(AR) /verbose /out:$(LIBG) $(OBJG)
endif



# Link a program
#
ifdef PROGO
obj/$(PROGO):   mkdir_obj  $(BOBJO)  $(BOBJO)
	@$(ECHO)  "   <<< Link:  $(PROGO) >>>"
	$(LD)  -OUT:obj/$(PROGO)  $(BOBJO)  $(LDLIB_O) $(LDLIB)
	$(MT)  -manifest  obj/$(PROGO).manifest -outputresource:obj/$(PROGO);#1
endif


ifdef PROGG
obj/$(PROGG):   mkdir_obj  $(BOBJG)  $(BOBJG)
	@$(ECHO)  "   <<< Link:  $(PROGG) >>>"
	$(LD)  -OUT:obj/$(PROGG)  $(BOBJG)           \
           $(LDFLAGS_G)                          \
           $(LDLIB_G)  $(LDLIB)
	$(MT)  -manifest  obj/$(PROGG).manifest -outputresource:obj/$(PROGG);#1
endif


#
#######################  end  common/vrg3d/win32.make  #########################
