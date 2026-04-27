#include "wavtool/AudioClip.h"
#include <stdexcept>
#include <cstdint>

namespace wavtool {
AudioClip::AudioClip(uint16_t audioFormat_, uint16_t channelCount_, uint32_t sampleRate_,
                uint32_t byteRate_, uint16_t blockAlign_, uint16_t bitDepth_, 
                const std::vector<int16_t>& leftChannel_, 
                const std::vector<int16_t>& rightChannel_,
                const std::string& clipName_, 
                const std::string& originalFilePath_) 
    
    : audioFormat(audioFormat_),
      channelCount(channelCount_),
      sampleRate(sampleRate_),
      byteRate(byteRate_),
      blockAlign(blockAlign_),
      bitDepth(bitDepth_),
      leftChannel(leftChannel_),
      rightChannel(rightChannel_),
      clipName(clipName_),
      originalFilePath(originalFilePath_)
{
    if (audioFormat != 1) {
        throw std::invalid_argument("Only PCM (format 1) is supported");
    }
    if (channelCount != 1 && channelCount != 2) {
        throw std::invalid_argument("Channel count must be 1 (mono) or 2 (stereo)");
    }
    if (sampleRate == 0) {
        throw std::invalid_argument("Sample rate must be greater than 0");
    }
    if (bitDepth != 16) {
        throw std::invalid_argument("Only 16-bit audio is supported");
    }
    if (leftChannel.empty()) {
        throw std::invalid_argument("Left channel must not be empty");
    }
    if (channelCount == 1) {
        if (!rightChannel.empty()) {
            throw std::invalid_argument("Mono audio should not have right channel data");
        }
    }
    if (channelCount == 2) {
        if (rightChannel.empty()) {
            throw std::invalid_argument("Stereo audio must have right channel data");
        }
        if (leftChannel.size() != rightChannel.size()) {
            throw std::invalid_argument("Left and right channels must be the same size");
        }
    }
    const uint16_t expectedBlockAlign = channelCount * static_cast<uint16_t>(bitDepth / 8);
    if (blockAlign != expectedBlockAlign) {
        throw std::invalid_argument("Block align does not match channel count and bit depth");
    }
    const uint32_t expectedByteRate = sampleRate * expectedBlockAlign;
    if (byteRate != expectedByteRate) {
        throw std::invalid_argument("Byte rate does not match sample rate, channels, and bit depth");
    }
}

const std::vector<int16_t>& AudioClip::getLeft() const {
    return leftChannel;
}

const std::vector<int16_t>& AudioClip::getRight() const {
    return rightChannel;
}

uint16_t AudioClip::getAudioFormat() const {
    return audioFormat;
}

uint16_t AudioClip::getChannelCount() const {
    return channelCount;
}

uint32_t AudioClip::getSampleRate() const {
    return sampleRate;
}

uint32_t AudioClip::getByteRate() const {
    return byteRate;
}

uint16_t AudioClip::getBlockAlign() const {
    return blockAlign;
}

uint16_t AudioClip::getBitDepth() const {
    return bitDepth;
}

size_t AudioClip::getFrameCount() const {
    return leftChannel.size();
}

double AudioClip::getDurationSeconds() const {
    return (static_cast<double>(getFrameCount()) / sampleRate);
}

bool AudioClip::isMono() const {
    return channelCount == 1;
}

bool AudioClip::isStereo() const {
    return channelCount == 2;
}

const std::string& AudioClip::getName() const {
    return clipName;
}

const std::string& AudioClip::getOriginalFilePath() const {
    return originalFilePath;
}
}
