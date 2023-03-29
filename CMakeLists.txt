cmake_minimum_required(VERSION 3.10)
project(SDL-template VERSION 1.0) 
set(CMAKE_CXX_STANDARD 14)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# SETUP VARIABLES:
set(CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

# INCLUDE FILES THAT SHOULD BE COMPILED:

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")
file(GLOB_RECURSE IMGUI "lib/imgui/*.cpp" "lib/imgui/*.h")

message(STATUS "Add source files:")
foreach(SRC_FILE IN LISTS SRC)
    message(STATUS "${SRC_FILE}")
endforeach()
message(STATUS "")

add_executable(${PROJECT_NAME} ${SRC} ${IMGUI})


IF (WIN32)

    # SETUP SDL2

    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
    find_package(SDL2 REQUIRED)
    if(NOT SDL2_FOUND)
        message( FATAL_ERROR "The SDL2 devel package must be located in Program Files (x86)" )

        # NOTE: Folders here contain both SDL2 and SDL2_image files.
        set(SDL2_LIB_DIR C:/SDL/lib)
        set(SDL2_INCLUDE_DIR C:/SDL/include)
        set(SDL2_LIBRARY C:/SDL/bin)
    endif()

    # SETUP INCLUDE AND LINK DIRS:

    include_directories(${SDL2_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})


    # COPY OVER STATIC FILES

    message(STATUS "Copying all dependencies to destination folder...")

    foreach(lib ${SDL2_LIBRARY})
        get_filename_component(FILE_EXT ${lib} EXT)
        if(FILE_EXT STREQUAL ".dll")
            list(APPEND PROJECT_DEPENDENCIES ${lib})
        endif()
    endforeach()
    

    foreach(Dependency IN LISTS PROJECT_DEPENDENCIES)
        file(
            COPY ${Dependency}
            DESTINATION ${OUTPUT_DIR}
        )
    endforeach()
ELSE()
    find_package(SDL2 REQUIRED)

    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
ENDIF()


message(STATUS "Copying all assets to destination folder...")
file(
    COPY assets
    DESTINATION ${OUTPUT_DIR}
)


target_include_directories(${PROJECT_NAME} PUBLIC lib/PerlinNoise)

