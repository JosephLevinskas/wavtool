cmake_minimum_required(VERSION 3.20)

project(Final-Wav-Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library
# Creates library target which is reusable
add_library(wavtool_core 
    src/AudioClip.cpp
    src/WavFile.cpp 
    src/AudioProcessor.cpp
    src/WaveFormGenerator.cpp
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



# make executable to test audioClip class from
add_executable(audio_clip_tests
    tests/AudioClipTests.cpp
)
# link so wavtool_tests can access code library
target_link_libraries(audio_clip_tests
    PRIVATE wavtool_core
)



# Testing exe for wav file save and load 
add_executable(wav_file_tests
    tests/WavFileTests.cpp
)
# link to wav testing
target_link_libraries(wav_file_tests
    PRIVATE wavtool_core
)



# Testing audioprocessor file
add_executable(audio_processor_tests
    tests/AudioProcessorTests.cpp
)

target_link_libraries(audio_processor_tests
    PRIVATE wavtool_core
)



add_executable(waveform_generator_tests
    tests/WaveformGeneratorTests.cpp
)

target_link_libraries(waveform_generator_tests
    PRIVATE wavtool_core
)