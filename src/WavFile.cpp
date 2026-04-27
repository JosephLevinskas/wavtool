#include "wavtool/WavFile.h"
#include "wavtool/AudioClip.h"
#include <stdexcept>
#include <cstdint>
#include <array>
#include <fstream>
#include <vector>

namespace wavtool {

    AudioClip WavFile::load(const std::filesystem::path& filePath) {
        std::ifstream file(filePath, std::ios::binary);

        if (!file) {
            throw std::runtime_error("Could not open WAV file");
        }

        std::array<char, 4> riffId;
        uint32_t riffSize = 0;
        std::array<char, 4> waveId;

        file.read(riffId.data(), riffId.size());
        file.read(reinterpret_cast<char*>(&riffSize), sizeof(riffSize));
        file.read(waveId.data(), waveId.size());

        if (!file) {
            throw std::runtime_error("Invalid WAV file: incomplete RIFF header");
        }
        if (riffId != std::array<char, 4>{'R', 'I', 'F', 'F'}) {
            throw std::runtime_error("Invalid WAV file: missing RIFF header");
        }
        if (waveId != std::array<char, 4>{'W', 'A', 'V', 'E'}) {
            throw std::runtime_error("Invalid WAV file: missing wave format");
        }

        bool foundFmt = false;
        bool foundData = false;

        std::streampos dataStartPosition;
        uint32_t dataSizeBytes = 0;

        std::array<char, 4> chunkId;
        uint32_t chunkSize = 0;

        uint16_t audioFormat = 0;
        uint16_t channelCount = 0;
        uint32_t sampleRate = 0;
        uint32_t byteRate = 0;
        uint16_t blockAlign = 0;
        uint16_t bitDepth = 0;

        while (!foundFmt || !foundData) {
            file.read(chunkId.data(), chunkId.size());
            file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
            
            if (!file) {
                throw std::runtime_error("Invalid WAV file: missing fmt or data chunk");
            }

            if (chunkId == std::array<char, 4> {'f', 'm', 't', ' '}) {
                if (chunkSize < 16) {
                    throw std::runtime_error("Invalid WAV file: fmt chunk too small");
                }
                
                file.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));
                file.read(reinterpret_cast<char*>(&channelCount), sizeof(channelCount));
                file.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
                file.read(reinterpret_cast<char*>(&byteRate), sizeof(byteRate));
                file.read(reinterpret_cast<char*>(&blockAlign), sizeof(blockAlign));
                file.read(reinterpret_cast<char*>(&bitDepth), sizeof(bitDepth));
                
                if (!file) {
                    throw std::runtime_error("Invalid WAV file: incomplete fmt chunk");
                }
                
                foundFmt = true;
                file.seekg(chunkSize - 16, std::ios::cur);
            }

            else if (chunkId == std::array<char, 4> {'d', 'a', 't', 'a'}) {
                dataStartPosition = file.tellg();
                dataSizeBytes = chunkSize;
                foundData = true;
                file.seekg(chunkSize + (chunkSize % 2), std::ios::cur);
            }
            else {
                file.seekg(chunkSize + (chunkSize % 2), std::ios::cur);
            }

        }

        if (audioFormat != 1) {
            throw std::runtime_error("Unsupported WAV file: only PCM format is supported");
        }

        if (channelCount != 1 && channelCount != 2) {
            throw std::runtime_error("Unsupported WAV file: only mono and stereo are supported");
        }

        if (bitDepth != 16) {
            throw std::runtime_error("Unsupported WAV file: only 16-bit audio is supported");
        }

        file.seekg(dataStartPosition);
        std::vector<int16_t> leftChannel;
        std::vector<int16_t> rightChannel;

        if (blockAlign == 0 || dataSizeBytes % blockAlign != 0) {
            throw std::runtime_error("Incorrect block alignment in Data section");
        }

        uint32_t frameCount = dataSizeBytes / blockAlign;
        
        if (channelCount == 1) {
            leftChannel.reserve(frameCount);
            int16_t curr = 0;
            for (uint32_t i = 0; i < frameCount; i++) {
                file.read(reinterpret_cast<char*>(&curr), sizeof(curr));
                leftChannel.push_back(curr);
            }
        } else if (channelCount == 2) {
            leftChannel.reserve(frameCount);
            rightChannel.reserve(frameCount);
            int16_t curr1 = 0;
            int16_t curr2 = 0;
            for (uint32_t i = 0; i < frameCount; i++) {
                file.read(reinterpret_cast<char*>(&curr1), sizeof(curr1));
                file.read(reinterpret_cast<char*>(&curr2), sizeof(curr2));
                leftChannel.push_back(curr1);
                rightChannel.push_back(curr2);
            }
        }

        if (!file) {
            throw std::runtime_error("Invalid WAV file: incomplete sample data");
        }

        return AudioClip(
            audioFormat,
            channelCount,
            sampleRate,
            byteRate,
            blockAlign,
            bitDepth,
            leftChannel,
            rightChannel,
            filePath.filename().string(),
            filePath.string()
        );
    }

    void WavFile::save(const AudioClip& clip, const std::filesystem::path& filePath) {
        std::ofstream file(filePath, std::ios::binary);

        if (!file) {
            throw std::runtime_error("Could not open file for writing");
        }

        uint32_t dataSizeBytes = clip.getFrameCount() * clip.getBlockAlign();
        uint32_t riffSize = 36 + dataSizeBytes;

        file.write("RIFF", 4);
        file.write(reinterpret_cast<const char*>(&riffSize), sizeof(riffSize));
        file.write("WAVE", 4);

        file.write("fmt ", 4);
        uint32_t fmtChunkSize = 16;
        file.write(reinterpret_cast<const char*>(&fmtChunkSize), sizeof(fmtChunkSize));
        
        uint16_t audioFormat = clip.getAudioFormat();
        uint16_t channelCount = clip.getChannelCount();
        uint32_t sampleRate = clip.getSampleRate();
        uint32_t byteRate = clip.getByteRate();
        uint16_t blockAlign = clip.getBlockAlign();
        uint16_t bitDepth = clip.getBitDepth();

        file.write(reinterpret_cast<const char*>(&audioFormat), sizeof(audioFormat));
        file.write(reinterpret_cast<const char*>(&channelCount), sizeof(channelCount));
        file.write(reinterpret_cast<const char*>(&sampleRate), sizeof(sampleRate));
        file.write(reinterpret_cast<const char*>(&byteRate), sizeof(byteRate));
        file.write(reinterpret_cast<const char*>(&blockAlign), sizeof(blockAlign));
        file.write(reinterpret_cast<const char*>(&bitDepth), sizeof(bitDepth));

        file.write("data", 4);
        file.write(reinterpret_cast<const char*>(&dataSizeBytes), sizeof(dataSizeBytes));

        if (clip.isMono()) {
            const auto& left = clip.getLeft();
            for (uint32_t i = 0; i < left.size(); ++i) {
                file.write(reinterpret_cast<const char*>(&left[i]), sizeof(uint16_t));
            }
        } else if (clip.isStereo()) {
            const auto& left = clip.getLeft();
            const auto& right = clip.getRight();
            for (uint32_t i = 0; i < left.size(); ++i) {
                file.write(reinterpret_cast<const char*>(&left[i]), sizeof(int16_t));
                file.write(reinterpret_cast<const char*>(&right[i]), sizeof(int16_t));
            }
        }

        if (!file) {
            throw std::runtime_error("Failed while writing WAV file");
        }
        

    }



}