cmake_minimum_required(VERSION 3.10)
project(eVTOL_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(eVTOL_sim
    src/main.cpp
)

# Find installed GoogleTest package
find_package(GTest REQUIRED)

# Enable testing support
enable_testing()

# Test executable
add_executable(eVTOL_tests
    tests/test_main.cpp
)

# Link test executable with GTest libraries
target_link_libraries(eVTOL_tests PRIVATE GTest::gtest GTest::gtest_main)

# Register test with CTest
add_test(NAME eVTOL_tests COMMAND eVTOL_tests)
