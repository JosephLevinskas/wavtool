
#include "wavtool/WaveformGenerator.h"
#include <vector>
#include <stdexcept>
#include <cmath>

namespace wavtool {

    std::vector<WaveformPoint> WaveformGenerator::generate(const AudioClip& clip, size_t requestedColumnCount) {
        if (requestedColumnCount < 1) {
            throw std::invalid_argument("Column count is too low to generate a waveform");
        }
        
        size_t actualColumnCount = std::min(requestedColumnCount, clip.getFrameCount());
        std::vector<WaveformPoint> columnMinMax;
        columnMinMax.reserve(actualColumnCount);

        const auto& left = clip.getLeft();
        const auto& right = clip.getRight();

        size_t frameCount = clip.getFrameCount();
        bool stereo = clip.isStereo();

        for (size_t i = 0; i < actualColumnCount; i++) {
            size_t startFrame = i * frameCount / actualColumnCount;
            size_t endFrame = (i + 1) * frameCount / actualColumnCount;
            int16_t max = left[startFrame];
            int16_t min = left[startFrame];

            for (size_t j = startFrame; j < endFrame; j++) {
                int16_t tempL = left[j];
                if (tempL < min) {
                    min = tempL;
                } else if (tempL > max) {
                    max = tempL;
                }
                
                if (stereo) {
                    int16_t tempR = right[j];
                    if (tempR < min) {
                        min = tempR;
                    } else if (tempR > max) {
                        max = tempR;
                    }
                }
            }
            WaveformPoint curr;
            curr.maxAmp = max;
            curr.minAmp = min;
            columnMinMax.push_back(curr);
        }
        return columnMinMax;
    }



}