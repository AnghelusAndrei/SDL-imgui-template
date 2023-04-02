
SET(SDL2_IMAGE_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${SDL2_IMAGE_PATH}
    "C:/Program Files (x86)/SDL2_image"
	"C:/Program Files (x86)"
    "C:/Program Files"
    "C:"
)

FIND_PATH(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
	HINTS
	$ENV{SDL2IMAGEDIR}
	PATH_SUFFIXES include/SDL2_image include
	PATHS ${SDL2_IMAGE_SEARCH_PATHS}
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
	set(PATH_SUFFIXES lib64 lib/x64 lib)
else() 
	set(PATH_SUFFIXES lib/x86 lib)
endif() 

FIND_FILE(SDL2_IMAGE_DLL SDL2_image.dll 	
    PATH_SUFFIXES ${PATH_SUFFIXES}
    PATHS ${SDL2_IMAGE_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2_IMAGE_LIBRARY_TEMP
	NAMES SDL2_image
	HINTS
	$ENV{SDL2IMAGEDIR}
	PATH_SUFFIXES ${PATH_SUFFIXES}
	PATHS ${SDL2_IMAGE_SEARCH_PATHS}
)

list(APPEND SDL2_IMAGE_LIBRARY_TEMP ${SDL2_IMAGE_DLL})



IF(SDL2_IMAGE_LIBRARY_TEMP)

	# For OS X, SDL2_imageuses Cocoa as a backend so it must link to Cocoa.
	# CMake doesn't display the -framework Cocoa string in the UI even
	# though it actually is there if I modify a pre-used variable.
	# I think it has something to do with the CACHE STRING.
	# So I use a temporary variable until the end so I can set the
	# "real" variable in one-shot.
	IF(APPLE)
		SET(SDL2_IMAGE_LIBRARY_TEMP ${SDL2_IMAGE_LIBRARY_TEMP} "-framework Cocoa")
	ENDIF(APPLE)

	# For threads, as mentioned Apple doesn't need this.
	# In fact, there seems to be a problem if I used the Threads package
	# and try using this line, so I'm just skipping it entirely for OS X.
	IF(NOT APPLE)
		SET(SDL2_IMAGE_LIBRARY_TEMP ${SDL2_IMAGE_LIBRARY_TEMP} ${CMAKE_THREAD_LIBS_INIT})
	ENDIF(NOT APPLE)

	# For MinGW library
	IF(MINGW)
		SET(SDL2_IMAGE_LIBRARY_TEMP ${MINGW32_LIBRARY} ${SDL2_IMAGE_LIBRARY_TEMP})
	ENDIF(MINGW)

	# Set the final string here so the GUI reflects the final state.
	SET(SDL2_IMAGE_LIBRARY ${SDL2_IMAGE_LIBRARY_TEMP} CACHE STRING "Where the SDL2_imageLibrary can be found")
	# Set the temp variable to INTERNAL so it is not seen in the CMake GUI
	SET(SDL2_IMAGE_LIBRARY_TEMP "${SDL2_IMAGE_LIBRARY_TEMP}" CACHE INTERNAL "")
ENDIF(SDL2_IMAGE_LIBRARY_TEMP)

# message("</FindSDL2.cmake>")

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2_image REQUIRED_VARS SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR)