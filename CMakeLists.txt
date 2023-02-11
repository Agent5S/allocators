cmake_minimum_required(VERSION 3.0.0)
project(Allocators VERSION 0.1.0)

include(CTest)
enable_testing()

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib)

set(SOURCE 
    ${HEADERS}
    source/TempAlloc.c
    source/ChunkAlloc.c
)
set(HEADERS 
    headers
)

add_library(allocators SHARED ${SOURCE})
target_include_directories(allocators PUBLIC ${HEADERS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
