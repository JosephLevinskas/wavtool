#ifndef WAVTOOL_WAVEFORMGENERATOR_H
#define WAVTOOL_WAVEFORMGENERATOR_H

#include <vector>
#include "wavtool/AudioClip.h"

namespace wavtool {

struct WaveformPoint {
    int16_t maxAmp;
    int16_t minAmp;
};

class WaveformGenerator {
public:
    static std::vector<WaveformPoint> generate(const AudioClip& clip, size_t requestedColumnCount);
};
}

#endif