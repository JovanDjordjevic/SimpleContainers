if(SC_ENABLE_BUILD_EXAMPLES)
    set(SC_TEST_SOURCES "simpleRingBufferExample.cpp")

    foreach(SC_SOURCE IN LISTS SC_TEST_SOURCES)
        get_filename_component(SC_EXAMPLE_NAME ${SC_SOURCE} NAME_WLE)
        message(STATUS "Creating example: ${SC_EXAMPLE_NAME} from source: ${SC_SOURCE}")
        
        add_executable(${SC_EXAMPLE_NAME} ${SC_SOURCE})
        target_link_libraries(${SC_EXAMPLE_NAME} PUBLIC simpleContainers)
    endforeach()
endif()
