cmake_minimum_required(VERSION 2.8)

enable_testing()

include_directories(${ROOT_DIRECTORY}/src)
include_directories(${DEP_INCLUDE})
include_directories(${DEP_INCLUDE}/libevent)

link_directories(${DEP_LIB})


file(GLOB_RECURSE testfiles RELATIVE "${ROOT_DIRECTORY}/tests" ${ROOT_DIRECTORY}/tests/*.cpp)

foreach(testfile IN LISTS testfiles)
    get_filename_component(testname ${testfile} NAME_WE)
    add_executable(${testname} ${testfile})
    target_link_libraries(${testname} dflog fmt common mysqlclient)
    message(STATUS "test file name : ${testname}")
endforeach()
