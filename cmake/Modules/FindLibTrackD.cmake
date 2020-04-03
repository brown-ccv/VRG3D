find_path(TRACKD_INCLUDE_DIR trackdAPI_c.h
		HINTS $ENV{G}/src/mechdyne/trackdAPI/include)

find_library(LIBTRACKD_LIBRARY NAMES libtrackdAPI_c.a
		HINTS $ENV{G}/src/mechdyne/trackdAPI/lib64/linux_x86_64)

set (LIBTRACKD_ALL_LIBRARIES ${LIBTRACKD_LIBRARY})
set (LIBTRACKD_INCLUDE_DIRS ${TRACKD_INCLUDE_DIR}) 

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBTRACKD DEFAULT_MSG
		LIBTRACKD_LIBRARY TRACKD_INCLUDE_DIR)

mark_as_advanced(LIBTRACKD_INCLUDE_DIRS LIBTRACKD_ALL_LIBRARIES)
