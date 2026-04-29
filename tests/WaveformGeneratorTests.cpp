#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "wavtool/AudioClip.h"
#include "wavtool/WaveformGenerator.h"

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
    std::vector<int16_t> left = {
        -10, 20, -30, 40,
        -50, 60, -70, 80
    };

    return wavtool::AudioClip(
        1, 1, 4, 8, 2, 16,
        left, {},
        "mono.wav",
        "test_assets/mono.wav"
    );
}

wavtool::AudioClip makeStereoClip() {
    std::vector<int16_t> left = {
        -10, 20, -30, 40,
        -50, 60, -70, 80
    };

    std::vector<int16_t> right = {
        100, -200, 300, -400,
        500, -600, 700, -800
    };

    return wavtool::AudioClip(
        1, 2, 4, 16, 4, 16,
        left, right,
        "stereo.wav",
        "test_assets/stereo.wav"
    );
}

void testGenerateMonoExactColumns() {
    std::cout << "Running testGenerateMonoExactColumns...\n";

    auto clip = makeMonoClip();

    auto waveform = wavtool::WaveformGenerator::generate(clip, 4);

    assert(waveform.size() == 4);

    assert(waveform[0].minAmp == -10);
    assert(waveform[0].maxAmp == 20);

    assert(waveform[1].minAmp == -30);
    assert(waveform[1].maxAmp == 40);

    assert(waveform[2].minAmp == -50);
    assert(waveform[2].maxAmp == 60);

    assert(waveform[3].minAmp == -70);
    assert(waveform[3].maxAmp == 80);

    std::cout << "Passed testGenerateMonoExactColumns\n";
}

void testGenerateMonoOneColumn() {
    std::cout << "Running testGenerateMonoOneColumn...\n";

    auto clip = makeMonoClip();

    auto waveform = wavtool::WaveformGenerator::generate(clip, 1);

    assert(waveform.size() == 1);
    assert(waveform[0].minAmp == -70);
    assert(waveform[0].maxAmp == 80);

    std::cout << "Passed testGenerateMonoOneColumn\n";
}

void testGenerateStereoCombinedChannels() {
    std::cout << "Running testGenerateStereoCombinedChannels...\n";

    auto clip = makeStereoClip();

    auto waveform = wavtool::WaveformGenerator::generate(clip, 4);

    assert(waveform.size() == 4);

    assert(waveform[0].minAmp == -200);
    assert(waveform[0].maxAmp == 100);

    assert(waveform[1].minAmp == -400);
    assert(waveform[1].maxAmp == 300);

    assert(waveform[2].minAmp == -600);
    assert(waveform[2].maxAmp == 500);

    assert(waveform[3].minAmp == -800);
    assert(waveform[3].maxAmp == 700);

    std::cout << "Passed testGenerateStereoCombinedChannels\n";
}

void testRequestedColumnsGreaterThanFrames() {
    std::cout << "Running testRequestedColumnsGreaterThanFrames...\n";

    auto clip = makeMonoClip();

    auto waveform = wavtool::WaveformGenerator::generate(clip, 100);

    assert(waveform.size() == clip.getFrameCount());

    for (size_t i = 0; i < waveform.size(); i++) {
        assert(waveform[i].minAmp == clip.getLeft()[i]);
        assert(waveform[i].maxAmp == clip.getLeft()[i]);
    }

    std::cout << "Passed testRequestedColumnsGreaterThanFrames\n";
}

void testUnevenFrameDistribution() {
    std::cout << "Running testUnevenFrameDistribution...\n";

    std::vector<int16_t> left = {1, 2, 3, 4, 5};

    wavtool::AudioClip clip(
        1, 1, 5, 10, 2, 16,
        left, {},
        "uneven.wav"
    );

    auto waveform = wavtool::WaveformGenerator::generate(clip, 2);

    assert(waveform.size() == 2);

    assert(waveform[0].minAmp == 1);
    assert(waveform[0].maxAmp == 2);

    assert(waveform[1].minAmp == 3);
    assert(waveform[1].maxAmp == 5);

    std::cout << "Passed testUnevenFrameDistribution\n";
}

void testInvalidZeroColumnsThrows() {
    std::cout << "Running testInvalidZeroColumnsThrows...\n";

    auto clip = makeMonoClip();

    assertThrowsInvalidArgument([&]() {
        wavtool::WaveformGenerator::generate(clip, 0);
    });

    std::cout << "Passed testInvalidZeroColumnsThrows\n";
}

int main() {
    testGenerateMonoExactColumns();
    testGenerateMonoOneColumn();
    testGenerateStereoCombinedChannels();
    testRequestedColumnsGreaterThanFrames();
    testUnevenFrameDistribution();
    testInvalidZeroColumnsThrows();

    std::cout << "All WaveformGenerator tests passed.\n";
    return 0;
}