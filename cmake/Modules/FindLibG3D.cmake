find_path(LIBG3D_INCLUDE_DIR G3D/G3D.h
             HINTS $ENV{G}/src/G3D/G3D-8.0/include $ENV{G3D_INCLUDE_DIR} $ENV{G}/install_linux/include ${G3D_INCLUDE_DIR})

find_library(LIB1_GLG3D NAMES libGLG3D.a GLG3D.lib GLG3D
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

find_library(LIB2_G3D NAMES libG3D.a G3D.lib G3D
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

# find_library(LIB3_GLG3Dd NAMES libGLG3Dd.a GLG3Dd.lib GLG3Dd
#              HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

# find_library(LIB4_G3Dd NAMES libG3Dd.a G3Dd.lib G3Dd
#              HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

find_library(LIB10_z NAMES libz.a z.lib z zlib
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} ${G3D_LIB_DIR})

find_library(LIB11_zip NAMES libzip.a zip.lib zip
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} ${G3D_LIB_DIR})

find_library(LIB12_png NAMES libpng.a png.lib png
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} ${G3D_LIB_DIR})

find_library(LIB13_jpeg NAMES libjpeg.a jpeg.lib jpeg
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} ${G3D_LIB_DIR})

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

	find_library(LIB7_avformat NAMES libavformat.a avformat.lib avformat
				 HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

	find_library(LIB8_avcodec NAMES libavcodec.a avcodec.lib avcodec
				 HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

	find_library(LIB9_avutil NAMES libavutil.a avtuil.lib avutil
				 HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

else()
	if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
		find_library(LIB_MINGWRT NAMES mingwrt.lib mingwrt
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR})
			 
		find_library(LIB7_avformat NAMES libavformat.a avformat.lib avformat
		  HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/ ${G3D_LIB_DIR})

		find_library(LIB8_avcodec NAMES libavcodec.a avcodec.lib avcodec
			HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

		find_library(LIB9_avutil NAMES libavutil.a avtuil.lib avutil
			HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib  ${G3D_LIB_DIR})
	elseif (COMPILE_WITH_64FFMPEG)
		find_library(LIB7_avformat NAMES libavformat.a avformat.lib avformat HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib  ${G3D_LIB_DIR})

		find_library(LIB8_avcodec NAMES libavcodec.a avcodec.lib avcodec HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib  ${G3D_LIB_DIR})

		find_library(LIB9_avutil NAMES libavutil.a avtuil.lib avutil HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} $ENV{G}/install_linux/lib ${G3D_LIB_DIR})

		find_library(LIB10_swscale NAMES libswscale.a swscale.lib swscale HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} ${G3D_LIB_DIR})
	endif() 
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

find_library(LIB14_pthread NAMES libpthread.a pthread.lib pthread
             HINTS $ENV{G}/src/G3D/G3D-8.0/${GBUILDSTR} $ENV{G3D_LIB_DIR} ${G3D_LIB_DIR})

endif()

set(LIBG3D_DEBUG_LIBRARIES debug ${LIB1_GLG3D} debug ${LIB2_G3D})
set(LIBG3D_OPT_LIBRARIES optimized ${LIB1_GLG3D} optimized ${LIB2_G3D})
set(LIBG3D_ALL_LIBRARIES ${LIB10_z} ${LIB11_zip} ${LIB12_png} ${LIB13_jpeg})
set(LIBG3D_INCLUDE_DIRS ${LIBG3D_INCLUDE_DIR} )

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(LIBG3D_ALL_LIBRARIES ${LIBG3D_ALL_LIBRARIES} ${LIB7_avformat} ${LIB8_avcodec} ${LIB9_avutil})
else() 
	if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(LIBG3D_ALL_LIBRARIES ${LIBG3D_ALL_LIBRARIES} ${LIB7_avformat} ${LIB8_avcodec} ${LIB9_avutil} ${LIB_MINGWRT})
	elseif (COMPILE_WITH_64FFMPEG)
		set(LIBG3D_ALL_LIBRARIES ${LIBG3D_ALL_LIBRARIES} ${LIB7_avformat} ${LIB8_avcodec} ${LIB9_avutil} ${LIB10_swscale})
	endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	find_library(LIBCOCOA Cocoa)
	set(LIBG3D_ALL_LIBRARIES ${LIBG3D_ALL_LIBRARIES} ${LIBCOCOA})
endif()

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(LIBG3D_ALL_LIBRARIES ${LIBG3D_ALL_LIBRARIES} ${LIB14_pthread})
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBNAME_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBG3D  DEFAULT_MSG
                                  LIB1_GLG3D LIB2_G3D LIB10_z LIB11_zip LIB12_png LIB13_jpeg LIBG3D_INCLUDE_DIR)

if(NOT LIBG3D_FOUND)
  set(G3D_INCLUDE_DIR $ENV{G3D_INCLUDE_DIR} CACHE PATH "Set the directory location of the G3D include folder")
  set(G3D_LIB_DIR $ENV{G3D_LIB_DIR} CACHE PATH "Set the directory location of the G3D libs folder")
endif()

mark_as_advanced(LIB1_GLG3D LIB2_G3D LIB10_z LIB11_zip LIB12_png LIB13_jpeg LIBG3D_INCLUDE_DIR LIBG3D_OPT_LIBRARY_DIR LIBG3D_DEBUG_LIBRARY_DIR )
