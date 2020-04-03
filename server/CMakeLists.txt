cmake_minimum_required (VERSION 2.6)
set (CMAKE_VERBOSE_MAKEFILE TRUE)

add_executable (vrg3d_server vrg3d_server.cpp)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  target_link_libraries (vrg3d_server debug vrg3d ${LIBS_DEBUG} ${LIBS_ALL} optimized vrg3d ${LIBS_OPT} ${LIBS_ALL} X11 pthread)
else()
  target_link_libraries (vrg3d_server debug vrg3d ${LIBS_DEBUG} ${LIBS_ALL} optimized vrg3d ${LIBS_OPT} ${LIBS_ALL})
endif ()
