set(GLUT_INCLUDE_DIR $ENV{GLUT_INCLUDE_DIR} CACHE PATH "Set the directory location of the Glut include folder")
set(GLUT_LIB_DIR $ENV{GLUT_LIB_DIR} CACHE PATH "Set the directory location of the Glut lib folder")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	find_path(LIBGLUT_INCLUDE_DIR GLUT/glut.h
		  HINTS $ENV{GLUT_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
else()
	find_path(LIBGLUT_INCLUDE_DIR GL/glut.h
		  HINTS $ENV{GLUT_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
endif()

find_library(LIBGLUT_LIBRARY NAMES libglut.a glut.lib glut
             HINTS $ENV{GLUT_LIB_DIR} ${GLUT_LIB_DIR})

set(LIBGLUT_ALL_LIBRARIES ${LIBGLUT_LIBRARY})
set(LIBGLUT_INCLUDE_DIRS ${LIBGLUT_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBGLUT  DEFAULT_MSG
                                  LIBGLUT_INCLUDE_DIR LIBGLUT_LIBRARY)

mark_as_advanced(LIBGLUT_INCLUDE_DIR LIBGLUT_LIBRARY )
