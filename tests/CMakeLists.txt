if(SC_ENABLE_BUILD_TESTS)
    set(SC_TEST_SOURCES 
        # "simpleRingBufferTest.cpp"
        "simpleHashedArrayTreeTest.cpp"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(SC_ENABLE_BRUTAL_COMPILE_OPTIONS)
            include(brutal-compiler-options)

            set(SC_WARNING_SUPPRESSORS)

            if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                list(APPEND SC_WARNING_SUPPRESSORS 
                    -Wno-unsafe-loop-optimizations
                )
            elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                list(APPEND SC_WARNING_SUPPRESSORS 
                    -Wno-padded
                    -Wno-date-time
                    -Wno-c++98-compat
                )
            elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                list(APPEND SC_WARNING_SUPPRESSORS 
                    /wd4820
                    /wd4514
                    /wd4711
                    /wd5045
                )
            endif()
        endif()

        if(SC_ENABLE_SANITIZERS)
            include(sanitize)
        endif()

        if(SC_ENABLE_CALLGRIND_TARGETS)
            include(grind)
        endif()

        if(SC_ENABLE_STATIC_ANALYSIS)
            include(static-analysis)
        endif()
    endif()

    foreach(SC_SOURCE IN LISTS SC_TEST_SOURCES)
        get_filename_component(SC_TEST_NAME ${SC_SOURCE} NAME_WLE)
        message(STATUS "Creating test: ${SC_TEST_NAME} from source: ${SC_SOURCE}")

        add_executable(${SC_TEST_NAME} ${SC_SOURCE})
        target_link_libraries(${SC_TEST_NAME} PUBLIC simpleContainers)

        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            if(SC_ENABLE_BRUTAL_COMPILE_OPTIONS)
                scm_add_brutal_compiler_options(${SC_TEST_NAME} PUBLIC ${SC_WARNING_SUPPRESSORS})
            endif()

            if(SC_ENABLE_SANITIZERS)
                # scm_add_address_sanitizer_with_options(${SC_TEST_NAME} PUBLIC)
                # scm_add_undefined_behavior_sanitizer_with_options(${SC_TEST_NAME} PUBLIC)
                # scm_add_thread_sanitizer_with_options(${SC_TEST_NAME} PUBLIC)
                # scm_add_memory_sanitizer_with_options(${SC_TEST_NAME} PUBLIC)
            endif()

            if(SC_ENABLE_CALLGRIND_TARGETS)
                scm_create_grind_target(${SC_TEST_NAME})
            endif()

            if(SC_ENABLE_STATIC_ANALYSIS)
                # this target_include_directories is only needed for the ${SC-TEST_NAME}-static-analyze target
                # because at this time it cannot extract include directories from the linked simpleContainers target
                # remove this line once that is working!
                target_include_directories(${SC_TEST_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

                scm_add_static_analysis_target(${SC_TEST_NAME})
            endif()
        endif()

        add_test(NAME ${SC_TEST_NAME} COMMAND ${SC_TEST_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

        if(SC_ENABLE_COVERAGE_REPORT)
            target_code_coverage(${SC_TEST_NAME} AUTO ALL)
        endif()
    endforeach()
endif()
