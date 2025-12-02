cmake_minimum_required(VERSION 3.10)
project(LimitOrderBook VERSION 1.0)

# Set C++ Standard to C++17 or C++20 (needed for modern features)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include the header files
include_directories(include)

# Gather all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the Main Executable
add_executable(LOB_Engine ${SOURCES})

# --- Setup for Testing (Optional for now) ---
# enable_testing()
# add_subdirectory(tests)