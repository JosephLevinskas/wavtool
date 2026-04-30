#include "wavtool/WavFile.h"
#include "wavtool/AudioProcessor.h"
#include "wavtool/WaveformGenerator.h"
#include "wavtool/Renderer.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

template <typename Func>
auto timeStep(const std::string& label, Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();

    auto result = func();

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << label << ": " << ms << " ms\n";

    return result;
}

void timeVoidStep(const std::string& label, const auto& func) {
    auto start = std::chrono::high_resolution_clock::now();

    func();

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << label << ": " << ms << " ms\n";
}

void printClipInfo(const wavtool::AudioClip& clip) {
    std::cout << "\nClip info\n";
    std::cout << "Name: " << clip.getName() << '\n';
    std::cout << "Original path: " << clip.getOriginalFilePath() << '\n';
    std::cout << "Channels: " << clip.getChannelCount() << '\n';
    std::cout << "Sample rate: " << clip.getSampleRate() << '\n';
    std::cout << "Bit depth: " << clip.getBitDepth() << '\n';
    std::cout << "Frame count: " << clip.getFrameCount() << '\n';
    std::cout << "Duration: " << clip.getDurationSeconds() << " seconds\n";
}

} // namespace

int main() {
    try {
        std::filesystem::path inputPath = "test_assets/tokyo-rain.wav";
        std::filesystem::path outputDir = "test_output";

        std::filesystem::create_directories(outputDir);

        std::filesystem::path copyPath = outputDir / "stress_copy.wav";
        std::filesystem::path reversedPath = outputDir / "stress_reversed.wav";
        std::filesystem::path trimmedPath = outputDir / "stress_trimmed.wav";
        std::filesystem::path normalizedPath = outputDir / "stress_normalized.wav";
        std::filesystem::path renderedPath = outputDir / "stress_waveform.bmp";

        std::cout << "Starting WavTool stress test\n\n";

        auto clip = timeStep("Load original WAV", [&] {
            return wavtool::WavFile::load(inputPath);
        });

        printClipInfo(clip);

        timeVoidStep("Save copy WAV", [&] {
            wavtool::WavFile::save(clip, copyPath);
        });

        auto copiedClip = timeStep("Reload copied WAV", [&] {
            return wavtool::WavFile::load(copyPath);
        });

        auto reversedClip = timeStep("Reverse audio", [&] {
            return wavtool::AudioProcessor::reverse(copiedClip);
        });

        timeVoidStep("Save reversed WAV", [&] {
            wavtool::WavFile::save(reversedClip, reversedPath);
        });

        double trimStart = 10.0;
        double trimEnd = 40.0;

        if (clip.getDurationSeconds() < trimEnd) {
            trimStart = 0.0;
            trimEnd = clip.getDurationSeconds() / 2.0;
        }

        auto trimmedClip = timeStep("Trim audio", [&] {
            return wavtool::AudioProcessor::trim(clip, trimStart, trimEnd);
        });

        timeVoidStep("Save trimmed WAV", [&] {
            wavtool::WavFile::save(trimmedClip, trimmedPath);
        });

        auto normalizedClip = timeStep("Normalize audio", [&] {
            return wavtool::AudioProcessor::normalize(clip);
        });

        timeVoidStep("Save normalized WAV", [&] {
            wavtool::WavFile::save(normalizedClip, normalizedPath);
        });

        auto waveform = timeStep("Generate waveform", [&] {
            return wavtool::WaveformGenerator::generate(clip, 3000);
        });

        timeVoidStep("Render waveform BMP", [&] {
            wavtool::Renderer::saveWaveformBmp(
                waveform,
                800,
                wavtool::RgbColor{0, 120, 255},
                renderedPath
            );
        });

        auto finalReload = timeStep("Reload normalized WAV", [&] {
            return wavtool::WavFile::load(normalizedPath);
        });

        printClipInfo(finalReload);

        std::cout << "\nStress test complete\n";
        std::cout << "Outputs written to: " << outputDir << '\n';

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nStress test failed: " << e.what() << '\n';
        return 1;
    }
}