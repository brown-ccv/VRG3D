#.SILENT:

# Instructions for using this Makefile
#
# The default target only really does a configure. Do 'make' and then 
# 'make linux' to do the linux build.  Also, this build requires G3D 8.01
# and some version of VRPN. If these are already installed, specify them
# with G3D_LIB_DIR, G3D_INCLUDE_DIR, VRPN_LIB_DIR and VRPN_INCLUDE_DIR or 
# via the $G tree.
#
# If G3D is not installed, run 'make G3D-8.0' to install it in the
# G3D-8.0 subdirectory here.
#

ARCH:=$(shell bin/arch)
ifeq ($(strip $(CMAKE)),)
CMAKE:=cmake
endif

ifeq ($(G),)
ifeq ($(G3D_LIB_DIR),)
G3D_INCLUDE_DIR:=G3D-8.0/include
G3D_LIB_DIR:=G3D-8.0/lib
export G3D_INCLUDE_DIR
export G3D_LIB_DIR
endif
endif

# Default rule: compile the latest version from svn
all: cmake-build

cmake-build:
	-mkdir build
        ifeq ($(ARCH),linux)
	  -mkdir build/Release
	  -mkdir build/Debug	  
	  cd build/Release; $(CMAKE) -DCMAKE_BUILD_TYPE=Release ../../
	  cd build/Debug; $(CMAKE) -DCMAKE_BUILD_TYPE=Debug ../../
        endif
        ifeq ($(ARCH),WIN32)
	  cd build; $(CMAKE) ../ -G "Visual Studio 10 Win64"
	  @echo "Go to the build directory and make your project via the Visual Studio solution file"
        endif
        ifeq ($(ARCH),OSX)
	  cd build; $(CMAKE) ../ -G Xcode
        endif

cmake-64ffmpeg:
	-mkdir build
        ifeq ($(ARCH),linux)
	  -mkdir build/Release
	  -mkdir build/Debug	  
	  cd build/Release; $(CMAKE) -DCMAKE_BUILD_TYPE=Release -DCOMPILE_WITH_64FFMPEG=ON ../../
	  cd build/Debug; $(CMAKE) -DCMAKE_BUILD_TYPE=Debug -DCOMPILE_WITH_64FFMPEG=ON ../../
        endif
        ifeq ($(ARCH),WIN32)
	  cd build; $(CMAKE) -DCOMPILE_WITH_64FFMPEG=ON ../ -G "Visual Studio 10 Win64"
	  @echo "Go to the build directory and make your project via the Visual Studio solution file"
        endif
        ifeq ($(ARCH),OSX)
	  cd build; $(CMAKE) -DCOMPILE_WITH_64FFMPEG=ON ../ -G Xcode
        endif

G3D-8.0: G3D
	cd G3D; cmake .; make install

linux:
	cd build/Release; make
	cd build/Debug; make

linux-install:
	cd build/Release; make install
	cd build/Debug; make install

clean:  
	rm -rf build/

