cmake_minimum_required(VERSION 3.10)
project(eVTOL_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for executables to the project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Main executable
add_executable(eVTOL_sim
    src/main.cpp
    src/vehicle.cpp
    src/std_rng.cpp
    src/simulation.cpp
    src/logger.cpp
)

# Find installed GoogleTest package
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Prevent GoogleTest from installing itself
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


# Enable testing support
enable_testing()

# Test executable
add_executable(eVTOL_tests
    tests/test_main.cpp
    tests/test_vehicle.cpp
    src/vehicle.cpp
    src/std_rng.cpp
    src/simulation.cpp
    src/logger.cpp
)

target_include_directories(eVTOL_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Link test executable with GTest libraries
target_link_libraries(eVTOL_tests PRIVATE GTest::gtest_main GTest::gmock)

# Register test with CTest
add_test(NAME eVTOL_tests COMMAND eVTOL_tests)
