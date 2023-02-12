list(APPEND TEST_TARGETS ChunkAllocatorTests)

list(APPEND TEST_LIBS ${CMOCKA_LIBRARIES})
list(APPEND TEST_LIBS allocators)

list(APPEND TEST_DIRS ${CMOCKA_INCLUDE_DIRS})
list(APPEND TEST_DIRS ${SOURCE})

foreach(TEST_TARGET IN LISTS TEST_TARGETS)
    add_executable(
        ${TEST_TARGET}
        ${TEST_TARGET}.c
    )
    target_link_libraries(
        ${TEST_TARGET}
        PUBLIC ${TEST_LIBS}
    )
    target_include_directories(
        ${TEST_TARGET}
        PUBLIC ${TEST_DIRS}
    )
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endforeach()