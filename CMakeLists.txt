cmake_minimum_required(VERSION 3.20)

project(WavTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core library
add_library(wavtool_core
    src/AudioClip.cpp
    src/WavFile.cpp
    src/AudioProcessor.cpp
    src/WaveformGenerator.cpp
    src/Renderer.cpp
    src/Library.cpp
    src/Project.cpp
    src/ProjectSerializer.cpp
    src/App.cpp
)

target_include_directories(wavtool_core
    PUBLIC
        include
)

# CLI app
add_executable(wavtool_cli
    apps/cli/main.cpp
)

target_link_libraries(wavtool_cli
    PRIVATE
        wavtool_core
)

# Tests
enable_testing()

add_executable(audio_clip_tests
    tests/AudioClipTests.cpp
)

target_link_libraries(audio_clip_tests
    PRIVATE
        wavtool_core
)

add_executable(wav_file_tests
    tests/WavFileTests.cpp
)

target_link_libraries(wav_file_tests
    PRIVATE
        wavtool_core
)

add_executable(audio_processor_tests
    tests/AudioProcessorTests.cpp
)

target_link_libraries(audio_processor_tests
    PRIVATE
        wavtool_core
)

add_executable(waveform_generator_tests
    tests/WaveformGeneratorTests.cpp
)

target_link_libraries(waveform_generator_tests
    PRIVATE
        wavtool_core
)

add_executable(renderer_tests
    tests/RendererTests.cpp
)

target_link_libraries(renderer_tests
    PRIVATE
        wavtool_core
)

add_executable(project_tests
    tests/ProjectTests.cpp
)

target_link_libraries(project_tests
    PRIVATE
        wavtool_core
)

add_test(NAME audio_clip_tests COMMAND audio_clip_tests)
add_test(NAME wav_file_tests COMMAND wav_file_tests)
add_test(NAME audio_processor_tests COMMAND audio_processor_tests)
add_test(NAME waveform_generator_tests COMMAND waveform_generator_tests)
add_test(NAME renderer_tests COMMAND renderer_tests)
add_test(NAME project_tests COMMAND project_tests)

set_tests_properties(audio_clip_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set_tests_properties(wav_file_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set_tests_properties(audio_processor_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set_tests_properties(waveform_generator_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set_tests_properties(renderer_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set_tests_properties(project_tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)