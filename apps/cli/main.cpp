#include <iostream>
#include <vector>
#include "wavtool/AudioClip.h"
#include "wavtool/WavFile.h"
#include "wavtool/AudioProcessor.h"




int main() {
    try {
        std::filesystem::path input = "test_assets/tokyo-rain.wav";

        auto clip = wavtool::WavFile::load(input);
        auto normalized = wavtool::AudioProcessor::normalize(clip);

        std::filesystem::create_directories("test_output");

        std::filesystem::path output = "test_output";
        output /= normalized.getName();

        wavtool::WavFile::save(normalized, output);

        int maxAbs = 0;

        for (auto sample : normalized.getLeft()) {
            maxAbs = std::max(maxAbs, std::abs(static_cast<int>(sample)));
        }

        if (normalized.isStereo()) {
            for (auto sample : normalized.getRight()) {
                maxAbs = std::max(maxAbs, std::abs(static_cast<int>(sample)));
            }
        }

        std::cout << "Loaded: " << clip.getName() << "\n";
        std::cout << "Normalized name: " << normalized.getName() << "\n";
        std::cout << "Original duration: " << clip.getDurationSeconds() << "\n";
        std::cout << "Normalized duration: " << normalized.getDurationSeconds() << "\n";
        std::cout << "Original frames: " << clip.getFrameCount() << "\n";
        std::cout << "Normalized frames: " << normalized.getFrameCount() << "\n";
        std::cout << "Max absolute amplitude after normalize: " << maxAbs << "\n";
        std::cout << "Saved to: " << output << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}