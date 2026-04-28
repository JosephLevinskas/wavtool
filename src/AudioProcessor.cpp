#include "wavtool/AudioProcessor.h"
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>
#include <cstdint>
#include <stdexcept>

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

    AudioClip AudioProcessor::trim(const AudioClip& clip, double startSeconds, double endSeconds) {
        if ((startSeconds < 0) || (endSeconds < 0) || (startSeconds >= endSeconds) || (endSeconds > clip.getDurationSeconds())) {
            throw std::invalid_argument("Invalid time(s) provided");
        }
        
        const auto& left = clip.getLeft();
        const auto& right = clip.getRight();

        size_t startFrame = static_cast<size_t>(clip.getSampleRate() * startSeconds);
        size_t endFrame = static_cast<size_t>(clip.getSampleRate() * endSeconds);

        if (startFrame >= endFrame) {
            throw std::invalid_argument("Time not enough distance apart");
        }

        std::vector<int16_t> newLeft(left.begin() + startFrame, left.begin() + endFrame);
        std::vector<int16_t> newRight(endFrame - startFrame);
        
        if (clip.isStereo()) {
            newRight.resize(endFrame - startFrame);
            auto start = right.begin() + startFrame;
            auto end = right.begin() + endFrame;
            std::copy(start, end, newRight.begin());
        }

        std::filesystem::path p = clip.getName();

        std::string newName =
            p.stem().string() + "_trimmed" + p.extension().string();

        AudioClip newClip = AudioClip(clip.getAudioFormat(),
                                    clip.getChannelCount(),
                                    clip.getSampleRate(),
                                    clip.getByteRate(),
                                    clip.getBlockAlign(),
                                    clip.getBitDepth(),
                                    newLeft,
                                    newRight,
                                    newName,
                                    clip.getOriginalFilePath());
        
        return newClip;

    }
}