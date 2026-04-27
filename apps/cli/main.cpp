#include <iostream>
#include <vector>
#include "wavtool/AudioClip.h"

int main() {
    std::vector<int16_t> left = {0, 1000, -1000, 500};

    wavtool::AudioClip clip(
        1,
        1,
        44100,
        88200,
        2,
        16,
        left,
        {},
        "Test Clip"
    );

    std::cout << clip.getName() << '\n';
    std::cout << clip.getFrameCount() << '\n';
    std::cout << clip.getDurationSeconds() << '\n';
    std::cout << clip.isMono() << '\n';

    return 0;
}