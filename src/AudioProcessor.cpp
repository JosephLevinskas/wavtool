#include "wavtool/AudioProcessor.h"
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>
#include <cstdint>
#include <stdexcept>
#include <cmath>

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

    AudioClip AudioProcessor::normalize(const AudioClip& clip) {
        const auto& left = clip.getLeft();
        const auto& right = clip.getRight();

        int maxAbs = 0;

        for (size_t i = 0; i < left.size(); i++) {
            int magnitude = std::abs(static_cast<int>(left[i]));
            if (magnitude > maxAbs) {
                maxAbs = magnitude;
            }
        }
        if (clip.isStereo()) {
            for (size_t i = 0; i < right.size(); i++) {
                int magnitude = std::abs(static_cast<int>(right[i]));
                if (magnitude > maxAbs) {
                    maxAbs = magnitude;
                }
            }
        }

        if (maxAbs == 0) {
            std::filesystem::path p = clip.getName();

            std::string newName =
                p.stem().string() + "_normalized_no_change" + p.extension().string();
                
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

        double scale = 32767.0 / maxAbs;
        std::vector<int16_t> newLeft(left.size());
        std::vector<int16_t> newRight;

        for (size_t i = 0; i < left.size(); i++) {
            double tempNewLeft = left[i] * scale;
            if (tempNewLeft > 32767) {
                tempNewLeft = 32767;
            }
            else if (tempNewLeft < -32768) {
                tempNewLeft = -32768;
            }
            newLeft[i] = int16_t(std::round(tempNewLeft));
        }
        if (clip.isStereo()) {
            newRight.resize(right.size()); 
            for (size_t i = 0; i < right.size(); i++) {
                double tempNewRight = right[i] * scale;
                if (tempNewRight > 32767) {
                    tempNewRight = 32767;
                }
                else if (tempNewRight < -32768) {
                    tempNewRight = -32768;
                }
                newRight[i] = int16_t(std::round(tempNewRight));
            }
        }

        std::filesystem::path p = clip.getName();

            std::string newName =
                p.stem().string() + "_normalized" + p.extension().string();
                
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