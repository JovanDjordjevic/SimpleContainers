if(ENABLE_TESTING)
    set(TEST_SOURCES "simpleRingBufferTest.cpp")

    foreach(SOURCE IN LISTS TEST_SOURCES)
        get_filename_component(TEST_NAME ${SOURCE} NAME_WLE)
        # message(STATUS "Creating test: " ${TEST_NAME} " from source: " ${SOURCE})
        
        add_executable(${TEST_NAME} ${SOURCE})
        target_link_libraries(${TEST_NAME} PUBLIC simpleContainers)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
        # add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

        if(ENABLE_COVERAGE_REPORT)
            target_code_coverage(${TEST_NAME} AUTO ALL)
        endif()
    endforeach()
endif()
