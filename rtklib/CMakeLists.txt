CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# variables declaration

SET(PROJECT rtklib)

FILE(GLOB_RECURSE HEADERS "src/*.h")

# FILE(GLOB_RECURSE SOURCES "src/*.c")
#FILE(GLOB_RECURSE SOURCES "src/testrtklib.c")

LIST(APPEND SOURCES "src/rcv/binex.c")
LIST(APPEND SOURCES "src/rcv/crescent.c")
LIST(APPEND SOURCES "src/rcv/gw10.c")
LIST(APPEND SOURCES "src/rcv/javad.c")
LIST(APPEND SOURCES "src/rcv/novatel.c")
LIST(APPEND SOURCES "src/rcv/nvs.c")
LIST(APPEND SOURCES "src/rcv/rcvlex.c")
LIST(APPEND SOURCES "src/rcv/rt17.c")
#LIST(APPEND SOURCES "src/rcv/septentrio.c")
LIST(APPEND SOURCES "src/rcv/skytraq.c")
LIST(APPEND SOURCES "src/rcv/ss2.c")
LIST(APPEND SOURCES "src/rcv/ublox.c")

LIST(APPEND SOURCES "src/convkml.c")
LIST(APPEND SOURCES "src/convrnx.c")
LIST(APPEND SOURCES "src/datum.c")
LIST(APPEND SOURCES "src/download.c")
LIST(APPEND SOURCES "src/ephemeris.c")
LIST(APPEND SOURCES "src/geoid.c")
LIST(APPEND SOURCES "src/ionex.c")
LIST(APPEND SOURCES "src/lambda.c")
LIST(APPEND SOURCES "src/options.c")
LIST(APPEND SOURCES "src/pntpos.c")
LIST(APPEND SOURCES "src/postpos.c")
LIST(APPEND SOURCES "src/ppp.c")
LIST(APPEND SOURCES "src/ppp_ar.c")
LIST(APPEND SOURCES "src/preceph.c")
LIST(APPEND SOURCES "src/qzslex.c")
LIST(APPEND SOURCES "src/rcvraw.c")
LIST(APPEND SOURCES "src/rinex.c")
LIST(APPEND SOURCES "src/rtcm.c")
LIST(APPEND SOURCES "src/rtcm2.c")
LIST(APPEND SOURCES "src/rtcm3.c")
LIST(APPEND SOURCES "src/rtcm3e.c")
LIST(APPEND SOURCES "src/rtkcmn.c")
LIST(APPEND SOURCES "src/rtkpos.c")
LIST(APPEND SOURCES "src/sbas.c")
LIST(APPEND SOURCES "src/solution.c")
LIST(APPEND SOURCES "src/tle.c")

LIST(APPEND SOURCES "src/testrtklib.c")

#LIST(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/testrtklib.c")

LIST(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/rcv/septentrio.c")

# source grouping

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})

# project definition

PROJECT(${PROJECT} C)

# includes

foreach(DEPENDENCY ${DEPENDS_ON_PROJECTS})
	include_directories("../${DEPENDENCY}")
endforeach()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

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

# SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
    ${PROJECT}
    ${DEPENDS_ON_PROJECTS})

if (MSVC)
	target_link_libraries(${PROJECT} winmm.lib)
endif()
