#include <iostream>
#include <vector>
#include "wavtool/AudioClip.h"
#include "wavtool/WavFile.h"
#include "wavtool/AudioProcessor.h"



int main() {
try {
std::filesystem::path input = "test_assets/tokyo-rain.wav";

    auto clip = wavtool::WavFile::load(input);

    // Trim from 2s to 5s
    double start = 2.0;
    double end = 5.0;

    auto trimmed = wavtool::AudioProcessor::trim(clip, start, end);

    std::filesystem::create_directories("test_output");

    std::filesystem::path output = "test_output";
    output /= trimmed.getName();

    wavtool::WavFile::save(trimmed, output);

    std::cout << "Original duration: " << clip.getDurationSeconds() << "\n";
    std::cout << "Trimmed duration: " << trimmed.getDurationSeconds() << "\n";

    std::cout << "Original frames: " << clip.getFrameCount() << "\n";
    std::cout << "Trimmed frames: " << trimmed.getFrameCount() << "\n";

    std::cout << "Trimmed first sample (L): "
              << trimmed.getLeft().front() << "\n";

    std::cout << "Trimmed last sample (L): "
              << trimmed.getLeft().back() << "\n";

    std::cout << "Saved to: " << output << "\n";
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}

return 0;

}