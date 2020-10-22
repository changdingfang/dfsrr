cmake_minimum_required(VERSION 2.8)
project(dfsrr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

set(ROOT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXECUTABLE_OUTPUT_PATH ${ROOT_DIRECTORY}/build/bin)
set(LIBRARY_OUTPUT_PATH ${ROOT_DIRECTORY}/build/lib)

set(DEP_INCLUDE "${ROOT_DIRECTORY}/depend/compiled/include")
set(DEP_LIB "${ROOT_DIRECTORY}/depend/compiled/lib")

option(DEPEND_BUILD "depend build" OFF)

if (DEPEND_BUILD)
    # dflog fmt三方依赖源码编译
    message(STATUS "build depend")
    set(DEPEND_SRC "${ROOT_DIRECTORY}/depend/src")
    set(DEP_INCLUDE "${DEPEND_SRC}")
    set(DEP_LIB "${DEPEND_SRC}")
    set(DFLOG_SRC "${DEPEND_SRC}")
    add_subdirectory(${DFLOG_SRC}/fmt)
    add_subdirectory(${DFLOG_SRC}/dflog)
    message(STATUS "depend src: ${DEPEND_SRC}")
endif()

message(STATUS "depend build: ${DEPEND_BUILD}")

message(STATUS "DEP_INCLUDE: ${DEP_INCLUDE}")
message(STATUS "DEP_LIB: ${DEP_LIB}")

add_subdirectory(src/common)
add_subdirectory(src/dfsrr)
add_subdirectory(src/dfsrrTool)
add_subdirectory(module)
# add_subdirectory(tests)
