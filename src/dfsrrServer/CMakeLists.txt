cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SOURCE_FILES)

include_directories(${ROOT_DIRECTORY}/src)
include_directories(${DEP_INCLUDE})
include_directories(${DEP_INCLUDE}/libevent)

link_directories(${DEP_LIB})

add_executable(dfsrr-server ${SOURCE_FILES})
target_link_libraries(dfsrr-server dflog fmt common pthread mysqlclient)
