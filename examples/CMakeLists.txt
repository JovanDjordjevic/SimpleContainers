if(ENABLE_BUILD_EXAMPLES)
    set(TEST_SOURCES "simpleRingBufferExample.cpp")

    foreach(SOURCE IN LISTS TEST_SOURCES)
        get_filename_component(EXAMPLE_NAME ${SOURCE} NAME_WLE)
        message(STATUS "Creating example: " ${EXAMPLE_NAME} " from source: " ${SOURCE})
        
        add_executable(${EXAMPLE_NAME} ${SOURCE})
        target_link_libraries(${EXAMPLE_NAME} PUBLIC simpleContainers)
    endforeach()
endif()
