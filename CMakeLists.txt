cmake_minimum_required(VERSION 3.10)
project(LimitOrderBook VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# 1. Define the Library Sources (The Logic)
# NOTE: Only list .cpp files here. Header files (.hpp) are handled by include_directories.
set(LIBRARY_SOURCES
    src/core/OrderBook.cpp
    src/core/LimitLevel.cpp
    src/core/MatchingEngine.cpp
    # Add src/types/Order.cpp here ONLY if you moved implementation out of the .hpp
)

# 2. Build the Main Engine (Manual Test)
# We combine the Runner (main.cpp) + The Logic (LIBRARY_SOURCES)
add_executable(LOB
    src/main.cpp 
    ${LIBRARY_SOURCES}
)

# 3. Build the Benchmark (Speed Test)
# We combine the Runner (LatencyBenchmark.cpp) + The Logic (LIBRARY_SOURCES)
add_executable(LOB_Benchmark
    benchmark/LatencyBenchmark.cpp 
    ${LIBRARY_SOURCES}
)