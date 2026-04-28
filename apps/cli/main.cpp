#include <iostream>
#include <vector>
#include "wavtool/AudioClip.h"
#include "wavtool/WavFile.h"
#include "wavtool/AudioProcessor.h"
#include "wavtool/WaveformGenerator.h"



int main() {
try {
std::filesystem::path input = "test_assets/tokyo-rain.wav";

    auto clip = wavtool::WavFile::load(input);

    size_t requestedColumns = 1000;

    auto waveform = wavtool::WaveformGenerator::generate(clip, requestedColumns);

    std::cout << "Requested columns: " << requestedColumns << "\n";
    std::cout << "Actual columns: " << waveform.size() << "\n\n";

    std::cout << "First 10 waveform points:\n";

    for (size_t i = 0; i < 10 && i < waveform.size(); i++) {
        std::cout << "Column " << i
                  << " | Min: " << waveform[i].minAmp
                  << " | Max: " << waveform[i].maxAmp
                  << "\n";
    }

    std::cout << "\nDone.\n";
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}

return 0;

}