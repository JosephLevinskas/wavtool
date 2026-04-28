#include <iostream>
#include <vector>
#include "wavtool/AudioClip.h"
#include "wavtool/WavFile.h"
#include "wavtool/AudioProcessor.h"



int main() {
    try {
        std::filesystem::path input = "test_assets/tokyo-rain.wav";

        auto clip = wavtool::WavFile::load(input);

        auto reversed = wavtool::AudioProcessor::reverse(clip);

        std::filesystem::create_directories("test_output");

        std::filesystem::path output = "test_output";
        output /= reversed.getName();

        wavtool::WavFile::save(reversed, output);

        std::cout << "Loaded: " << clip.getName() << "\n";
        std::cout << "Reversed name: " << reversed.getName() << "\n";
        std::cout << "Saved to: " << output << "\n";

        std::cout << "Original first left sample: "
                  << clip.getLeft().front() << "\n";

        std::cout << "Reversed first left sample: "
                  << reversed.getLeft().front() << "\n";

        std::cout << "Original last left sample: "
                  << clip.getLeft().back() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}