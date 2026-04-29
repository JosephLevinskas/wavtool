#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "wavtool/AudioClip.h"
#include "wavtool/AudioProcessor.h"

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

wavtool::AudioClip makeMonoClip() {
    std::vector<int16_t> left = {100, -200, 300, -400};

    return wavtool::AudioClip(
        1, 1, 4, 8, 2, 16,
        left, {},
        "mono.wav",
        "test_assets/mono.wav"
    );
}

wavtool::AudioClip makeStereoClip() {
    std::vector<int16_t> left = {100, -200, 300, -400};
    std::vector<int16_t> right = {10, -20, 30, -40};

    return wavtool::AudioClip(
        1, 2, 4, 16, 4, 16,
        left, right,
        "stereo.wav",
        "test_assets/stereo.wav"
    );
}

int getMaxAbs(const wavtool::AudioClip& clip) {
    int maxAbs = 0;

    for (auto sample : clip.getLeft()) {
        maxAbs = std::max(maxAbs, std::abs(static_cast<int>(sample)));
    }

    if (clip.isStereo()) {
        for (auto sample : clip.getRight()) {
            maxAbs = std::max(maxAbs, std::abs(static_cast<int>(sample)));
        }
    }

    return maxAbs;
}

void testReverseMono() {
    std::cout << "Running testReverseMono...\n";

    auto clip = makeMonoClip();
    auto reversed = wavtool::AudioProcessor::reverse(clip);

    std::vector<int16_t> expectedLeft = {-400, 300, -200, 100};

    assert(reversed.isMono());
    assert(reversed.getLeft() == expectedLeft);
    assert(reversed.getRight().empty());
    assert(reversed.getName() == "mono_reversed.wav");

    assert(clip.getLeft()[0] == 100);

    std::cout << "Passed testReverseMono\n";
}

void testReverseStereo() {
    std::cout << "Running testReverseStereo...\n";

    auto clip = makeStereoClip();
    auto reversed = wavtool::AudioProcessor::reverse(clip);

    std::vector<int16_t> expectedLeft = {-400, 300, -200, 100};
    std::vector<int16_t> expectedRight = {-40, 30, -20, 10};

    assert(reversed.isStereo());
    assert(reversed.getLeft() == expectedLeft);
    assert(reversed.getRight() == expectedRight);
    assert(reversed.getName() == "stereo_reversed.wav");

    std::cout << "Passed testReverseStereo\n";
}

void testTrimMono() {
    std::cout << "Running testTrimMono...\n";

    auto clip = makeMonoClip();

    auto trimmed = wavtool::AudioProcessor::trim(clip, 0.25, 0.75);

    std::vector<int16_t> expectedLeft = {-200, 300};

    assert(trimmed.isMono());
    assert(trimmed.getLeft() == expectedLeft);
    assert(trimmed.getRight().empty());
    assert(trimmed.getFrameCount() == 2);
    assert(trimmed.getName() == "mono_trimmed.wav");

    std::cout << "Passed testTrimMono\n";
}

void testTrimStereo() {
    std::cout << "Running testTrimStereo...\n";

    auto clip = makeStereoClip();

    auto trimmed = wavtool::AudioProcessor::trim(clip, 0.25, 0.75);

    std::vector<int16_t> expectedLeft = {-200, 300};
    std::vector<int16_t> expectedRight = {-20, 30};

    assert(trimmed.isStereo());
    assert(trimmed.getLeft() == expectedLeft);
    assert(trimmed.getRight() == expectedRight);
    assert(trimmed.getFrameCount() == 2);
    assert(trimmed.getName() == "stereo_trimmed.wav");

    std::cout << "Passed testTrimStereo\n";
}

void testTrimFullClip() {
    std::cout << "Running testTrimFullClip...\n";

    auto clip = makeMonoClip();

    auto trimmed = wavtool::AudioProcessor::trim(
        clip,
        0.0,
        clip.getDurationSeconds()
    );

    assert(trimmed.getLeft() == clip.getLeft());
    assert(trimmed.getFrameCount() == clip.getFrameCount());

    std::cout << "Passed testTrimFullClip\n";
}

void testTrimInvalidNegativeStartThrows() {
    std::cout << "Running testTrimInvalidNegativeStartThrows...\n";

    auto clip = makeMonoClip();

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioProcessor::trim(clip, -1.0, 0.5);
    });

    std::cout << "Passed testTrimInvalidNegativeStartThrows\n";
}

void testTrimInvalidNegativeEndThrows() {
    std::cout << "Running testTrimInvalidNegativeEndThrows...\n";

    auto clip = makeMonoClip();

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioProcessor::trim(clip, 0.0, -1.0);
    });

    std::cout << "Passed testTrimInvalidNegativeEndThrows\n";
}

void testTrimStartAfterEndThrows() {
    std::cout << "Running testTrimStartAfterEndThrows...\n";

    auto clip = makeMonoClip();

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioProcessor::trim(clip, 0.75, 0.25);
    });

    std::cout << "Passed testTrimStartAfterEndThrows\n";
}

void testTrimEndPastDurationThrows() {
    std::cout << "Running testTrimEndPastDurationThrows...\n";

    auto clip = makeMonoClip();

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioProcessor::trim(clip, 0.0, clip.getDurationSeconds() + 1.0);
    });

    std::cout << "Passed testTrimEndPastDurationThrows\n";
}

void testTrimTooSmallRangeThrows() {
    std::cout << "Running testTrimTooSmallRangeThrows...\n";

    auto clip = makeMonoClip();

    assertThrowsInvalidArgument([&]() {
        wavtool::AudioProcessor::trim(clip, 0.25, 0.251);
    });

    std::cout << "Passed testTrimTooSmallRangeThrows\n";
}

void testNormalizeMono() {
    std::cout << "Running testNormalizeMono...\n";

    auto clip = makeMonoClip();
    auto normalized = wavtool::AudioProcessor::normalize(clip);

    assert(normalized.isMono());
    assert(normalized.getRight().empty());
    assert(normalized.getFrameCount() == clip.getFrameCount());
    assert(getMaxAbs(normalized) == 32767);
    assert(normalized.getName() == "mono_normalized.wav");

    std::cout << "Passed testNormalizeMono\n";
}

void testNormalizeStereoUsesBothChannels() {
    std::cout << "Running testNormalizeStereoUsesBothChannels...\n";

    std::vector<int16_t> left = {100, -200};
    std::vector<int16_t> right = {1000, -4000};

    wavtool::AudioClip clip(
        1, 2, 4, 16, 4, 16,
        left, right,
        "stereo_loud_right.wav"
    );

    auto normalized = wavtool::AudioProcessor::normalize(clip);

    assert(normalized.isStereo());
    assert(getMaxAbs(normalized) == 32767);
    assert(normalized.getFrameCount() == clip.getFrameCount());

    std::cout << "Passed testNormalizeStereoUsesBothChannels\n";
}

void testNormalizeSilentMono() {
    std::cout << "Running testNormalizeSilentMono...\n";

    std::vector<int16_t> left = {0, 0, 0};

    wavtool::AudioClip clip(
        1, 1, 44100, 88200, 2, 16,
        left, {},
        "silent.wav"
    );

    auto normalized = wavtool::AudioProcessor::normalize(clip);

    assert(normalized.isMono());
    assert(normalized.getLeft() == left);
    assert(normalized.getRight().empty());
    assert(normalized.getName() == "silent_normalized_no_change.wav");

    std::cout << "Passed testNormalizeSilentMono\n";
}

void testNormalizeSilentStereo() {
    std::cout << "Running testNormalizeSilentStereo...\n";

    std::vector<int16_t> left = {0, 0, 0};
    std::vector<int16_t> right = {0, 0, 0};

    wavtool::AudioClip clip(
        1, 2, 44100, 176400, 4, 16,
        left, right,
        "silent_stereo.wav"
    );

    auto normalized = wavtool::AudioProcessor::normalize(clip);

    assert(normalized.isStereo());
    assert(normalized.getLeft() == left);
    assert(normalized.getRight() == right);
    assert(normalized.getName() == "silent_stereo_normalized_no_change.wav");

    std::cout << "Passed testNormalizeSilentStereo\n";
}

int main() {
    testReverseMono();
    testReverseStereo();

    testTrimMono();
    testTrimStereo();
    testTrimFullClip();
    testTrimInvalidNegativeStartThrows();
    testTrimInvalidNegativeEndThrows();
    testTrimStartAfterEndThrows();
    testTrimEndPastDurationThrows();
    testTrimTooSmallRangeThrows();

    testNormalizeMono();
    testNormalizeStereoUsesBothChannels();
    testNormalizeSilentMono();
    testNormalizeSilentStereo();

    std::cout << "All AudioProcessor tests passed.\n";
    return 0;
}