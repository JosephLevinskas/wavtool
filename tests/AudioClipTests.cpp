#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "wavtool/AudioClip.h"

template <typename Function>
void assertThrowsInvalidArgument(Function function) {
    bool threw = false;

    try {
        function();
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
}

void testValidMono() {
    std::cout << "Running testValidMono...\n";

    std::vector<int16_t> left = {0, 1000, -1000, 500};

    wavtool::AudioClip clip(
        1, 1, 44100, 88200, 2, 16,
        left, {},
        "Mono Test",
        "test.wav"
    );

    assert(clip.isMono());
    assert(!clip.isStereo());
    assert(clip.getAudioFormat() == 1);
    assert(clip.getChannelCount() == 1);
    assert(clip.getSampleRate() == 44100);
    assert(clip.getByteRate() == 88200);
    assert(clip.getBlockAlign() == 2);
    assert(clip.getBitDepth() == 16);
    assert(clip.getFrameCount() == 4);
    assert(clip.getLeft() == left);
    assert(clip.getRight().empty());
    assert(clip.getName() == "Mono Test");
    assert(clip.getOriginalFilePath() == "test.wav");

    std::cout << "Passed testValidMono\n";
}

void testValidStereo() {
    std::cout << "Running testValidStereo...\n";

    std::vector<int16_t> left = {0, 1000, -1000, 500};
    std::vector<int16_t> right = {10, 2000, -2000, 250};

    wavtool::AudioClip clip(
        1, 2, 44100, 176400, 4, 16,
        left, right,
        "Stereo Test"
    );

    assert(!clip.isMono());
    assert(clip.isStereo());
    assert(clip.getFrameCount() == 4);
    assert(clip.getLeft() == left);
    assert(clip.getRight() == right);

    std::cout << "Passed testValidStereo\n";
}

void testDurationCalculation() {
    std::cout << "Running testDurationCalculation...\n";

    std::vector<int16_t> left = {1, 2, 3, 4};

    wavtool::AudioClip clip(
        1, 1, 4, 8, 2, 16,
        left, {},
        "Duration Test"
    );

    assert(std::abs(clip.getDurationSeconds() - 1.0) < 0.000001);

    std::cout << "Passed testDurationCalculation\n";
}

void testInvalidFormat() {
    std::cout << "Running testInvalidFormat...\n";

    std::vector<int16_t> left = {0, 1000, -1000, 500};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            2, 1, 44100, 88200, 2, 16,
            left, {},
            "Bad Format"
        );
    });

    std::cout << "Passed testInvalidFormat\n";
}

void testInvalidChannelCount() {
    std::cout << "Running testInvalidChannelCount...\n";

    std::vector<int16_t> left = {0, 1000};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 3, 44100, 264600, 6, 16,
            left, {},
            "Bad Channels"
        );
    });

    std::cout << "Passed testInvalidChannelCount\n";
}

void testInvalidSampleRate() {
    std::cout << "Running testInvalidSampleRate...\n";

    std::vector<int16_t> left = {0, 1000, -1000, 500};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 1, 0, 0, 2, 16,
            left, {},
            "Bad Sample Rate"
        );
    });

    std::cout << "Passed testInvalidSampleRate\n";
}

void testInvalidBitDepth() {
    std::cout << "Running testInvalidBitDepth...\n";

    std::vector<int16_t> left = {0, 1000};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 1, 44100, 44100, 1, 8,
            left, {},
            "Bad Bit Depth"
        );
    });

    std::cout << "Passed testInvalidBitDepth\n";
}

void testEmptyLeftChannel() {
    std::cout << "Running testEmptyLeftChannel...\n";

    std::vector<int16_t> left = {};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 1, 44100, 88200, 2, 16,
            left, {},
            "Empty Left"
        );
    });

    std::cout << "Passed testEmptyLeftChannel\n";
}

void testMonoWithRightChannelThrows() {
    std::cout << "Running testMonoWithRightChannelThrows...\n";

    std::vector<int16_t> left = {0, 1, 2};
    std::vector<int16_t> right = {0, 1, 2};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 1, 44100, 88200, 2, 16,
            left, right,
            "Mono With Right"
        );
    });

    std::cout << "Passed testMonoWithRightChannelThrows\n";
}

void testStereoMissingRightChannelThrows() {
    std::cout << "Running testStereoMissingRightChannelThrows...\n";

    std::vector<int16_t> left = {0, 1, 2};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 2, 44100, 176400, 4, 16,
            left, {},
            "Stereo Missing Right"
        );
    });

    std::cout << "Passed testStereoMissingRightChannelThrows\n";
}

void testStereoMismatchThrows() {
    std::cout << "Running testStereoMismatchThrows...\n";

    std::vector<int16_t> left = {0, 1000, -1000, 500};
    std::vector<int16_t> right = {1, 2};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 2, 44100, 176400, 4, 16,
            left, right,
            "Stereo Mismatch"
        );
    });

    std::cout << "Passed testStereoMismatchThrows\n";
}

void testInvalidBlockAlignThrows() {
    std::cout << "Running testInvalidBlockAlignThrows...\n";

    std::vector<int16_t> left = {0, 1, 2};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 1, 44100, 88200, 4, 16,
            left, {},
            "Bad Block Align"
        );
    });

    std::cout << "Passed testInvalidBlockAlignThrows\n";
}

void testInvalidByteRateThrows() {
    std::cout << "Running testInvalidByteRateThrows...\n";

    std::vector<int16_t> left = {0, 1, 2};

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioClip badClip(
            1, 1, 44100, 12345, 2, 16,
            left, {},
            "Bad Byte Rate"
        );
    });

    std::cout << "Passed testInvalidByteRateThrows\n";
}

void testSampleLimits() {
    std::cout << "Running testSampleLimits...\n";

    std::vector<int16_t> left = {-32768, 0, 32767};

    wavtool::AudioClip clip(
        1, 1, 44100, 88200, 2, 16,
        left, {},
        "Limits"
    );

    assert(clip.getLeft()[0] == -32768);
    assert(clip.getLeft()[2] == 32767);

    std::cout << "Passed testSampleLimits\n";
}

int main() {
    testValidMono();
    testValidStereo();
    testDurationCalculation();

    testInvalidFormat();
    testInvalidChannelCount();
    testInvalidSampleRate();
    testInvalidBitDepth();
    testEmptyLeftChannel();
    testMonoWithRightChannelThrows();
    testStereoMissingRightChannelThrows();
    testStereoMismatchThrows();
    testInvalidBlockAlignThrows();
    testInvalidByteRateThrows();

    testSampleLimits();

    std::cout << "All AudioClip tests passed.\n";
    return 0;
}