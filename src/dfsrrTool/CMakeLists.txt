cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SOURCE_FILES)

include_directories(${ROOT_DIRECTORY}/src)
include_directories(${ROOT_DIRECTORY}/module)
include_directories(${DEP_INCLUDE})

link_directories(${DEP_LIB})
link_directories(${DEP_LIB}/libevent)


add_executable(dfsrrTool ${SOURCE_FILES})
target_link_libraries(dfsrrTool dfmodule dflog fmt common pthread libsqlite3.a)
