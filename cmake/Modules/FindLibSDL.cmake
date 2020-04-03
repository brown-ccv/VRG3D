find_path(LIBSDL_INCLUDE_DIR SDL/SDL.h
          HINTS $ENV{SDL_INCLUDE_DIR} ${SDL_INCLUDE_DIR})

find_library(LIBSDL_LIBRARY NAMES SDL
             HINTS $ENV{SDL_LIB_DIR} ${SDL_LIB_DIR})

set(LIBSDL_ALL_LIBRARIES ${LIBSDL_LIBRARY})
set(LIBSDL_INCLUDE_DIRS ${LIBSDL_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBSDL  DEFAULT_MSG
                                  LIBSDL_INCLUDE_DIR LIBSDL_LIBRARY)

if(NOT LIBSDL_FOUND)
  set(SDL_INCLUDE_DIR $ENV{SDL_INCLUDE_DIR} CACHE PATH "Set the directory location of the SDL include folder")
  set(SDL_LIB_DIR $ENV{SDL_LIB_DIR} CACHE PATH "Set the directory location of the SDL lib folder")
endif()

mark_as_advanced(LIBSDL_INCLUDE_DIR LIBSDL_LIBRARY )
