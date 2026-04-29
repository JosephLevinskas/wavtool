#include <iostream>
#include <vector>
#include "wavtool/AudioClip.h"
#include "wavtool/WavFile.h"
#include "wavtool/AudioProcessor.h"
#include "wavtool/WaveformGenerator.h"
#include "wavtool/Renderer.h"



int main() {
    try {
        std::cout << "Loading WAV file...\n";

        // 1. Load audio
        wavtool::AudioClip clip = wavtool::WavFile::load("test_assets/tokyo-rain.wav");

        std::cout << "Generating waveform...\n";

        // 2. Generate waveform (choose width)
        size_t width = 1000;
        auto waveform = wavtool::WaveformGenerator::generate(clip, width);

        std::cout << "Rendering BMP...\n";

        // 3. Choose color (blue-ish)
        wavtool::RgbColor color{0, 100, 255};

        // 4. Choose height (your design rule)
        size_t height = waveform.size() / 4;

        // 5. Render output
        wavtool::Renderer::saveWaveformBmp(
            waveform,
            height,
            color,
            "test_output/testoutput.bmp"
        );

        std::cout << "SUCCESS: Output saved to testoutput.bmp\n";

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}