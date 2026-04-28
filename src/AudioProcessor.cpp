#include "wavtool/AudioProcessor.h"
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>

namespace wavtool {
    AudioClip AudioProcessor::reverse(const AudioClip& clip) {
        auto left = clip.getLeft();
        auto right = clip.getRight();
        
        std::reverse(left.begin(), left.end());
    
        if (clip.isStereo()) {
            std::reverse(right.begin(), right.end());
        }

        std::filesystem::path p = clip.getName();

        std::string newName =
            p.stem().string() + "_reversed" + p.extension().string();

        AudioClip newClip = AudioClip(clip.getAudioFormat(),
                                    clip.getChannelCount(),
                                    clip.getSampleRate(),
                                    clip.getByteRate(),
                                    clip.getBlockAlign(),
                                    clip.getBitDepth(),
                                    left,
                                    right,
                                    newName,
                                    clip.getOriginalFilePath());
        return newClip;
    }

}