CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# variables declaration

SET(PROJECT rinex2db)

SET(DEPENDS_ON_PROJECTS Common Greis)

FILE(GLOB_RECURSE HEADERS "include/*.h")

FILE(GLOB_RECURSE SOURCES "src/*.cpp")

FILE(GLOB_RECURSE MOC_SOURCES "*_automoc.cpp")

# source grouping

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})
source_group ("Generated Files" FILES ${MOC_SOURCES})

# project definition

PROJECT(${PROJECT} CXX)

# includes

foreach(DEPENDENCY ${DEPENDS_ON_PROJECTS})
	include_directories("../${DEPENDENCY}")
endforeach()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}")

# rtklib special things

include_directories("../rtklib/src")

add_definitions(-D_LIB)
add_definitions(-DEXTLEX)
add_definitions(-DENAGLO)
add_definitions(-DENAGAL)
add_definitions(-DENAQZS)
add_definitions(-DENACMP)
add_definitions(-DENALEO)

# linking

add_executable(${PROJECT} ${HEADERS} ${SOURCES} ${MOC_SOURCES})

SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
    ${PROJECT}
    ${DEPENDS_ON_PROJECTS}
    ${Boost_LIBRARIES}
    Qt5::Core
    Qt5::Sql)

# dependencies

add_dependencies(${PROJECT} ${DEPENDS_ON_PROJECTS})
