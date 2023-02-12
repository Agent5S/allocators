cmake_minimum_required(VERSION 3.0.0)
project(Allocators VERSION 0.1.0)

include(CTest)
enable_testing()

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib)

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

enable_testing()

# Add CMocka CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka)
find_package(cmocka 1.1.0 REQUIRED)
add_subdirectory(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
