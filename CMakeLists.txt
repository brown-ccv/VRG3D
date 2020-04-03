cmake_minimum_required (VERSION 2.8)
project (VRG3D)

#set(USE_GLUT true)
#set(USE_OMNIMAP true)
#set(USE_TRACKD true)
#set(GBUILDSTR linux-x86_64-g++-4.4.5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#if we are using OS X, set USE_GLUT to always be true
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(USE_GLUT true)
  if (${DARWIN_MAJOR_VERSION} MATCHES "12")
    # Mountain Lion doesn't include X11 anymore, so we'll assume the user is
    # using MacPorts and has X11 installed.
    include_directories("/opt/local/include/")
  endif (${DARWIN_MAJOR_VERSION} MATCHES "12")
endif()

#Enable C++11 features in XCode
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  #set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -stdlib=libc++")
endif()

option(COMPILE_WITH_64FFMPEG "Set to true to compile with support for 64 bit FFMPEG" OFF)
# Look for G3D Library.  If found use that one, otherwise use,
# the included library
find_package(LibG3D)
if(LIBG3D_FOUND)
  set(LIBS_OPT ${LIBS_OPT} ${LIBG3D_OPT_LIBRARIES})
  set(LIBS_DEBUG ${LIBS_DEBUG} ${LIBG3D_DEBUG_LIBRARIES})
  set(LIBS_ALL ${LIBS_ALL} ${LIBG3D_ALL_LIBRARIES})
  include_directories(${LIBG3D_INCLUDE_DIRS})
else()
  # Use the G3D library packaged with vrg3d
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND COMPILE_WITH_64FFMPEG)
    # Build the G3D-8.0 66ffmpeg branch
    add_subdirectory (branches/g3d-8.0-64ffmpeg-win)
  else()
    # Build with G3D-8.0
    add_subdirectory (G3D)
  endif()
  set(LIBS_OPT ${LIBS_OPT} optimized GLG3D optimized G3D)
  set(LIBS_DEBUG ${LIBS_DEBUG} debug GLG3D debug G3D)
  find_package(ZLIB QUIET)
  if (NOT ZLIB_FOUND)
    set (LIBS_ALL ${LIBS_ALL} zlib)
  else()
    set (LIBS_ALL ${LIBS_ALL} ${LIB10_z})
  endif()
  set (LIBS_ALL ${LIBS_ALL} zip png jpeg)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	find_library(LIBCOCOA Cocoa)
	set(LIBS_ALL ${LIBS_ALL} ${LIBCOCOA})
  endif()
  #set (LIBS_ALL ${LIBS_ALL} X11 pthread)
  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (LIBS_ALL ${LIBS_ALL} avformat avcodec avutil)
  else()
	if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	  set (LIBS_ALL ${LIBS_ALL} avformat avcodec avutil mingwrt)
	elseif(COMPILE_WITH_64FFMPEG)
	  set (LIBS_ALL ${LIBS_ALL} avformat avcodec avutil swscale)
        endif()
  endif()
  include_directories(${VRG3D_SOURCE_DIR}/G3D/G3D.lib/include)
  include_directories(${VRG3D_SOURCE_DIR}/G3D/GLG3D.lib/include)
endif()

find_package(LibVRPN REQUIRED)
add_definitions(-DUSE_VRPN)
set(LIBS_OPT ${LIBS_OPT} ${LIBVRPN_OPT_LIBRARIES})
set(LIBS_DEBUG ${LIBS_DEBUG} ${LIBVRPN_DEBUG_LIBRARIES})
set(LIBS_ALL ${LIBS_ALL} ${LIBVRPN_ALL_LIBRARIES})
include_directories(${LIBVRPN_INCLUDE_DIRS})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR USE_GLUT)
   find_package(LibSDL REQUIRED)
   set(LIBS_OPT ${LIBS_OPT} ${LIBSDL_OPT_LIBRARIES})
   set(LIBS_DEBUG ${LIBS_DEBUG} ${LIBSDL_DEBUG_LIBRARIES})
   set(LIBS_ALL ${LIBS_ALL} ${LIBSDL_ALL_LIBRARIES})
   include_directories(${LIBSDL_INCLUDE_DIRS})
endif()

if (USE_TRACKD)
   find_package(LibTrackD)
   add_definitions(-DUSE_TRACKD)
   set(LIBS_ALL ${LIBS_ALL} ${LIBTRACKD_ALL_LIBRARIES})
   include_directories(${LIBTRACKD_INCLUDE_DIRS})
endif()

if(USE_GLUT)
  find_package(LibGLUT REQUIRED)
  add_definitions(-DUSE_GLUT)
  set(LIBS_OPT ${LIBS_OPT} ${LIBGLUT_OPT_LIBRARIES})
  set(LIBS_DEBUG ${LIBS_DEBUG} ${LIBGLUT_DEBUG_LIBRARIES})
  set(LIBS_ALL ${LIBS_ALL} ${LIBGLUT_ALL_LIBRARIES})
  include_directories(${LIBGLUT_INCLUDE_DIRS})
endif()

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  set(LIBS_ALL ${LIBS_ALL} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
  include_directories(${OPENGL_INCLUDE_DIR})
endif()

find_package(LibTUIO)
if(LIBTUIO_FOUND)
  add_definitions(-DUSE_TUIO)
  set(LIBS_OPT ${LIBS_OPT} ${LIBTUIO_OPT_LIBRARIES})
  set(LIBS_DEBUG ${LIBS_DEBUG} ${LIBTUIO_DEBUG_LIBRARIES})
  set(LIBS_ALL ${LIBS_ALL} ${LIBTUIO_ALL_LIBRARIES})
  include_directories(${LIBTUIO_INCLUDE_DIRS})
endif()

find_package(LibSPACENAV)
if(LIBSPACENAV_FOUND)
  add_definitions(-DUSE_SPACENAV)
  set(LIBS_OPT ${LIBS_OPT} ${LIBSPACENAV_OPT_LIBRARIES})
  set(LIBS_DEBUG ${LIBS_DEBUG} ${LIBSPACENAV_DEBUG_LIBRARIES})
  set(LIBS_ALL ${LIBS_ALL} ${LIBSPACENAV_ALL_LIBRARIES})
  include_directories(${LIBSPACENAV_INCLUDE_DIRS})
endif()

#if we are on windows look and using omnimap, look for it
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND USE_OMNIMAP)
   find_package(LibOMNIMAP REQUIRED)
   if(LIBOMNIMAP_FOUND)
     add_definitions(-DUSE_OMNIMAP)
     include_directories(${LIBOMNIMAP_INCLUDE_DIRS})
     set(LIBS_ALL ${LIBS_ALL} ${LIBOMNIMAP_ALL_LIBRARIES})
   endif()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  find_package(LibPQLABS)
  if(LIBPQLABS_FOUND)
    add_definitions(-DUSE_PQLABS)
    include_directories(${LIBPQLABS_INCLUDE_DIRS})
    set(LIBS_ALL ${LIBS_ALL} ${LIBPQLABS_ALL_LIBRARIES})
  endif()
endif()

set (CMAKE_VERBOSE_MAKEFILE TRUE)

# Used for compiling in subdirectories of the project
include_directories (
  .
  ..
  ${VRG3D_SOURCE_DIR}
)

# Used for finding libraries when exe's are built, as for the demo
if(COMPILE_WITH_64FFMPEG)
  link_directories (
    ${VRG3D_BINARY_DIR}/G3D/lib
    ${G3D_SOURCE_DIR}/ffmpeg/lib/win64
    ${VRG3D_BINARY_DIR}/vrg3d
  )
else()
  link_directories (
    ${VRG3D_BINARY_DIR}/G3D/lib
    ${VRG3D_BINARY_DIR}/ffmpeg/lib
    ${VRG3D_BINARY_DIR}/vrg3d
  )
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
  # Linux specific code
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  # Windows specific code
  add_definitions(-DUSE_SPACENAV)

endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")


add_subdirectory (vrg3d)

add_subdirectory (demo)

add_subdirectory (server)

add_subdirectory (vrbase)

