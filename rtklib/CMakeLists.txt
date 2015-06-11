CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# variables declaration

SET(PROJECT rtklib)

FILE(GLOB_RECURSE HEADERS "src/*.h")

FILE(GLOB_RECURSE SOURCES "src/*.c")

# source grouping

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})

# project definition

PROJECT(${PROJECT} CXX)

# includes

foreach(DEPENDENCY ${DEPENDS_ON_PROJECTS})
	include_directories("../${DEPENDENCY}")
endforeach()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}")

# preprocessor definitions

add_definitions(-D_LIB)
add_definitions(-DEXTLEX)
add_definitions(-DENAGLO)
add_definitions(-DENAGAL)
add_definitions(-DENAQZS)
add_definitions(-DENACMP)
add_definitions(-DENALEO)

# linking

add_library(${PROJECT} STATIC ${HEADERS} ${SOURCES})

SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
    ${PROJECT}
    ${DEPENDS_ON_PROJECTS}
    ${Pthread})
