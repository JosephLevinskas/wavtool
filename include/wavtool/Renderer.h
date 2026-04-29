#ifndef WAVTOOL_RENDERER_H
#define WAVTOOL_RENDERER_H

#include <cstdint>
#include <vector>
#include <filesystem>
#include "wavtool/WaveformGenerator.h"

namespace wavtool {

struct RgbColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class Renderer {
public:
    static void saveWaveformBmp(const std::vector<WaveformPoint> &waveform, size_t height, 
                                RgbColor chosenColor, const std::filesystem::path& outputPath);
};

}


#endif