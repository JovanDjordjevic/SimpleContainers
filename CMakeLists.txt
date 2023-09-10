cmake_minimum_required(VERSION 3.16)

project(SimpleContainers VERSION 1.0.0 LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a build directory.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # NOTE: does nothing if generator is set to Visual Studio
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

# optional targets
option(ENABLE_DOXYGEN "Create doxygen documentation target" OFF)
option(ENABLE_TESTING "Build the test executables" OFF)
option(ENABLE_COVERAGE_REPORT "Create code coverage report for tests" OFF)
option(ENABLE_BUILD_EXAMPLES "Build examples" OFF)

if(ENABLE_COVERAGE_REPORT AND NOT ENABLE_TESTING)
    message(FATAL_ERROR "Code coverage only available when building tests")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(ENABLE_COVERAGE_REPORT)
        message(FATAL_ERROR "\nCoverage report generation only supported on GNU and Clang compilers")
    endif()
endif()

if(ENABLE_DOXYGEN)
    # To generate html report, build the 'docs' target
    include(docs)
endif()

if(ENABLE_COVERAGE_REPORT)
    # To generate html coverage report, build the 'ccov-all' target
    message(STATUS "Adding code coverage")
    set(CODE_COVERAGE ON)
    include(code-coverage)
    add_code_coverage_all_targets()
endif()

add_subdirectory(include)

if(ENABLE_TESTING)
    # run tests with ctest
    message(STATUS "Building tests")
    include(CTest)
    add_subdirectory(tests)
endif()

if(ENABLE_BUILD_EXAMPLES)
    # you can find example executables in build/examples/... (depending on the cmake generator)
    message(STATUS "Building examples")
    add_subdirectory(examples)
endif()
