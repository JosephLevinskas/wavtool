#ifndef WAVTOOL_AUDIOCLIP_H
#define WAVTOOL_AUDIOCLIP_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace wavtool {
    class AudioClip { // Owns validated audio sample data and metadata for editing/display.
    private:
        uint16_t audioFormat = 0;
        uint16_t channelCount = 0;
        uint32_t sampleRate = 0;
        uint32_t byteRate = 0;
        uint16_t blockAlign = 0;
        uint16_t bitDepth = 0;

        std::vector<int16_t> leftChannel;
        std::vector<int16_t> rightChannel;

        std::string clipName = " ";
        std::string originalFilePath = "";

    public:
        AudioClip(uint16_t audioFormat_, uint16_t channelCount_, uint32_t sampleRate_,
                uint32_t byteRate_, uint16_t blockAlign_, uint16_t bitDepth_, 
                const std::vector<int16_t>& leftChannel_, 
                const std::vector<int16_t>& rightChannel_,
                const std::string& clipName_ = "Untitled", 
                const std::string& originalFilePath_ = "");

        const std::vector<int16_t>& getLeft() const;
        const std::vector<int16_t>& getRight() const;

        uint16_t getAudioFormat() const;
        uint16_t getChannelCount() const;
        uint32_t getSampleRate() const;
        uint32_t getByteRate() const;
        uint16_t getBlockAlign() const;
        uint16_t getBitDepth() const;
        
        size_t getFrameCount() const;
        double getDurationSeconds() const;

        bool isMono() const;
        bool isStereo() const;

        const std::string& getName() const;
        const std::string& getOriginalFilePath() const;

    };
}


#endif