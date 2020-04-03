find_path(LIBOMNIMAP_INCLUDE_DIR omnimap.h
          HINTS "C:/Program files (x86)/Elumenati/OmniMap/include" $ENV{OMNIMAP_INCLUDE_DIR})

find_library(OMNILIB1 NAMES OmniMap.lib
          HINTS "C:/Program files (x86)/Elumenati/OmniMap/lib" $ENV{OMNIMAP_LIB_DIR})

find_library(OMNILIB2 NAMES glew32s.lib
          HINTS "C:/Program files (x86)/Elumenati/OmniMap/lib" $ENV{OMNIMAP_LIB_DIR})

find_library(OMNILIB3 NAMES GLAUX.lib
          HINTS "C:/Program files (x86)/Elumenati/OmniMap/lib" $ENV{OMNIMAP_LIB_DIR})

find_library(OMNILIB4 NAMES GLU32.lib
          HINTS "C:/Program files (x86)/Elumenati/OmniMap/lib" $ENV{OMNIMAP_LIB_DIR})

find_library(OMNILIB5 NAMES OPENGL32.lib
          HINTS "C:/Program files (x86)/Elumenati/OmniMap/lib" $ENV{OMNIMAP_LIB_DIR})

set(LIBOMNIMAP_ALL_LIBRARIES ${OMNILIB1} ${OMNILIB2} ${OMNILIB3} ${OMNILIB4} ${OMNILIB5})
set(LIBOMNIMAP_INCLUDE_DIRS ${LIBOMNIMAP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBOMNIMAP  DEFAULT_MSG
                                  OMNILIB1 OMNILIB2 OMNILIB3 OMNILIB4 OMNILIB5)

mark_as_advanced(OMNILIB1 OMNILIB2 OMNILIB3 OMNILIB4 OMNILIB5)