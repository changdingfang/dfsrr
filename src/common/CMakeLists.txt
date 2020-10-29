cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SOURCE_FILES)

include_directories(${ROOT_DIRECTORY}/src)
include_directories(${DEP_INCLUDE})
include_directories(${DEP_INCLUDE}/libevent)

link_directories(${DEP_LIB})
link_directories(${DEP_LIB}/libevent)
link_directories(${RUNTIME_LIB})



add_library(common STATIC ${SOURCE_FILES})
target_link_libraries(common 
    dflog 
    fmt 
    dl 
    libsqlite3.a 
    libevent.a 
    libevent_pthreads.a
    )
