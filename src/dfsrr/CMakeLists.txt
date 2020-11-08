cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SOURCE_FILES)

include_directories(${ROOT_DIRECTORY}/src)
include_directories(${ROOT_DIRECTORY}/module)
include_directories(${DEP_INCLUDE})
include_directories(${DEP_INCLUDE}/libevent)

link_directories(${DEP_LIB})
link_directories(${DEP_LIB}/libevent)


add_executable(dfsrr ${SOURCE_FILES})
target_link_libraries(dfsrr dfmodule dflog fmt common pthread libsqlite3.a)
