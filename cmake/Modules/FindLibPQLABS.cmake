find_path(PQLABS_INCLUDE_DIR PQMTClient.h
		HINTS $ENV{G}/src/pqlabs/include)

find_library(LIBPQLABS_LIBRARY NAMES PQMTClient.lib
		HINTS $ENV{G}/src/pqlabs/lib/x64)

set (LIBPQLABS_ALL_LIBRARIES ${LIBPQLABS_LIBRARY})
set (LIBPQLABS_INCLUDE_DIRS ${PQLABS_INCLUDE_DIR}) 

include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(LIBPQLABS DEFAULT_MSG
#		LIBPQLABS_LIBRARY PQLABS_INCLUDE_DIR)
find_package_handle_standard_args(LIBPQLABS DEFAULT_MSG
		PQLABS_INCLUDE_DIR)

mark_as_advanced(LIBPQLABS_INCLUDE_DIRS LIBPQLABS_ALL_LIBRARIES)
