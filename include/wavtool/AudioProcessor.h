#ifndef WAVTOOL_AUDIOPROCESSOR_H
#define WAVTOOL_AUDIOPROCESSOR_H

#include "wavtool/AudioClip.h"

namespace wavtool {
class AudioProcessor {
public:
    static AudioClip reverse(const AudioClip& clip);
    static AudioClip trim(const AudioClip& clip, double startSeconds, double endSeconds);
    static AudioClip normalize(const AudioClip& clip);
};
}

#endif