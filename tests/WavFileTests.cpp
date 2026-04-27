#include "wavtool/WavFile.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <chrono>

namespace fs = std::filesystem;



void testLoadWavFile() {
    auto clip = wavtool::WavFile::load("test_assets/tokyo-rain.wav");

    assert(clip.getChannelCount() == 2);
    assert(clip.getSampleRate() == 48000);
    assert(clip.getBitDepth() == 16);
    assert(clip.getFrameCount() == 11521152);
    assert(clip.getLeft().size() == clip.getFrameCount());
    assert(clip.getRight().size() == clip.getFrameCount());

    std::cout << "testLoadWavFile passed\n";
}

void testSaveAndReloadWavFile() {
    fs::create_directories("test_output");

    auto original = wavtool::WavFile::load("test_assets/tokyo-rain.wav");

    wavtool::WavFile::save(
        original,
        "test_output/tokyo-rain_copy.wav"
    );

    auto copy = wavtool::WavFile::load("test_output/tokyo-rain_copy.wav");

    assert(copy.getChannelCount() == original.getChannelCount());
    assert(copy.getSampleRate() == original.getSampleRate());
    assert(copy.getBitDepth() == original.getBitDepth());
    assert(copy.getFrameCount() == original.getFrameCount());
    assert(copy.getLeft().size() == original.getLeft().size());
    assert(copy.getRight().size() == original.getRight().size());

    std::cout << "testSaveAndReloadWavFile passed\n";
}

void testInvalidFileThrows() {
    bool didThrow = false;

    try {
        wavtool::WavFile::load("test_assets/not-a-real-file.wav");
    }
    catch (const std::runtime_error&) {
        didThrow = true;
    }

    assert(didThrow);

    std::cout << "testInvalidFileThrows passed\n";
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    testLoadWavFile();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Load time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
    start = std::chrono::high_resolution_clock::now();
    testSaveAndReloadWavFile();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Load time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
    start = std::chrono::high_resolution_clock::now();
    testInvalidFileThrows();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Load time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
    

    std::cout << "All WavFile tests passed\n";
    return 0;
}