cmake_minimum_required(VERSION 3.20)

project(Final-Wav-Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library
# Creates library target which is reusable
add_library(wavtool_core 
    src/AudioClip.cpp 
)

# Anyone using wavtool_core can include headers from /include
target_include_directories(wavtool_core 
    PUBLIC include 
)

# CLI app 
# Creates the executable file .exe
add_executable(wavtool_cli 
    apps/cli/main.cpp 
)

# Connects the executable and core library
# wavtool_cli uses wavtool_core
target_link_libraries(wavtool_cli 
    PRIVATE wavtool_core 
)

# make executable so run tests from
add_executable(wavtool_tests
    tests/AudioClipTests.cpp 
)
# link so wavtool_tests can access code library
target_link_libraries(wavtool_tests
    PRIVATE wavtool_core
)