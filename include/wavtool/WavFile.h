#ifndef WAVTOOL_WAVFILE_H
#define WAVTOOL_WAVFILE_H

#include "wavtool/AudioClip.h"
#include <filesystem>

namespace wavtool {

class WavFile {
public:
    static AudioClip load(const std::filesystem::path& filePath);
    static void save(const AudioClip& clip, const std::filesystem::path& filePath);
};

} // namespace wavtool


#endif