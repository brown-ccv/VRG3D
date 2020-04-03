cmake_minimum_required (VERSION 2.6)
set (CMAKE_VERBOSE_MAKEFILE TRUE)

set (SOURCEFILES CheckerboardStencil.cpp ConfigMap.cpp DisplayTile.cpp Event.cpp G3DOperators.cpp HorizontalStencil.cpp ICubeXDevice.cpp Midi.cpp MouseToTracker.cpp ProjectionVRCamera.cpp SynchedSystem.cpp VRApp.cpp VRApp_network.cpp VRPNAnalogDevice.cpp VRPNButtonDevice.cpp VRPNTrackerDevice.cpp InputDevice.cpp)

set (HEADERFILES CheckerboardStencil.h ConfigMap.h DisplayTile.h Event.h EventNet.h G3DOperators.h HorizontalStencil.h ICubeXDevice.h Midi.h MouseToTracker.h ProjectionVRCamera.h SynchedSystem.h VRApp.h VRPNAnalogDevice.h VRPNButtonDevice.h VRPNTrackerDevice.h InputDevice.h)

source_group("Header Files" FILES ${HEADERFILES})

if(LIBSPACENAV_FOUND)
  set (SOURCEFILES ${SOURCEFILES} SpaceNavDevice.cpp)
  set (HEADERFILES ${HEADERFILES} SpaceNavDevice.h)
endif()

if(LIBTUIO_FOUND)
  set (SOURCEFILES ${SOURCEFILES} TUIOEvents.cpp)
  set (HEADERFILES ${HEADERFILES} TUIOEvents.h)
endif()

if(LIBPQLABS_FOUND)
  set (SOURCEFILES ${SOURCEFILES} PQLabsEvents.cpp)
  set (HEADERFILES ${HEADERFILES} PQLabsEvents.h)
endif()

if(LIBTRACKD_FOUND)
  set (SOURCEFILES ${SOURCEFILES} TrackDClient.cpp)
  set (HEADERFILES ${HEADERFILES} TrackDClient.h)
endif()

if(LIBISENSE_FOUND)
  set (SOURCEFILES ${SOURCEFILES} ISenseDirect.cpp)
  set (HEADERFILES ${HEADERFILES} ISenseDirect.h)
endif()

if(USE_GLUT)
  set (SOURCEFILES ${SOURCEFILES} GlutWindow.cpp)
  set (HEADERFILES ${HEADERFILES} GlutWindow.h)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Mac OS X specific code
  set (SOURCEFILES ${SOURCEFILES} VRApp_osx.cpp Midi_linux.cpp)
  if(LIBSPACENAV_FOUND)
    set (SOURCEFILES ${SOURCEFILES} SpaceNavDevice_osx.cpp)
  endif()
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  set (SOURCEFILES ${SOURCEFILES} VRApp_linux.cpp Midi_linux.cpp)
  if(LIBSPACENAV_FOUND)
    set (SOURCEFILES ${SOURCEFILES} SpaceNavDevice_linux.cpp)
  endif()
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
 set (SOURCEFILES ${SOURCEFILES} VRApp_win32.cpp Midi_win32.cpp SpaceNavDevice_win32.cpp)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_library (vrg3d ${SOURCEFILES} ${HEADERFILES})


