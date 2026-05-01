#include "wavtool/App.h"
#include "wavtool/WavFile.h"
#include "wavtool/AudioProcessor.h"
#include "wavtool/WaveformGenerator.h"
#include "wavtool/Renderer.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <vector>

namespace wavtool {

void App::run() {
    std::cout << "WavTool CLI\n";
    std::cout << "Type 'help' for commands.\n";

    std::string line;

    while (running) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            break;
        }

        try {
            handleCommand(line);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }
}

void App::handleCommand(const std::string& line) {
    std::istringstream stream(line);

    std::string command;
    stream >> command;

    std::vector<std::string> args;
    std::string arg;

    while (stream >> arg) {
        args.push_back(arg);
    }

    if (command.empty()) return;

    if (command == "exit" || command == "quit") {
        running = false;
        return;
    }
    else if (command == "help") {
        printHelp();
    }
    else if (command == "load") {
        loadFile(args);
    }
    else if (command == "info") {
        printCurrentInfo();
    }
    else if (command == "reverse") {
        reverse();
    }
    else if (command == "save") {
        saveCurrent(args);
    }
    else if (command == "undo") {
        undo();
    }
    else if (command == "redo") {
        redo();
    }
    else if (command == "trim") {
        trim(args);
    }
    else if (command == "normalize") {
        normalize();
    }
    else if (command == "render") {
        renderWaveform(args);
    }
    else {
        std::cout << "Unknown command. Type 'help'.\n";
    }
}

void App::printHelp() const {
    std::cout << "Commands:\n";
    std::cout << "  load <path>\n";
    std::cout << "  info\n";
    std::cout << "  reverse\n";
    std::cout << "  trim <start> <end>\n";
    std::cout << "  normalize\n";
    std::cout << "  render <output> <columns> <height>\n";
    std::cout << "  save <path>\n";
    std::cout << "  undo\n";
    std::cout << "  redo\n";
    std::cout << "  exit\n";
}

void App::loadFile(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        throw std::runtime_error("Usage: load <path>");
    }

    std::filesystem::path path = args[0];

    AudioClip clip = WavFile::load(path);

    project.loadClip(std::move(clip));

    std::cout << "Loaded: " << path << "\n";
}

void App::printCurrentInfo() const {
    if (!project.hasClip()) {
        std::cout << "No clip loaded\n";
        return;
    }

    const auto& clip = project.getClip();

    std::cout << "Name: " << clip.getName() << "\n";
    std::cout << "Channels: " << clip.getChannelCount() << "\n";
    std::cout << "Sample Rate: " << clip.getSampleRate() << "\n";
    std::cout << "Bit Depth: " << clip.getBitDepth() << "\n";
    std::cout << "Frames: " << clip.getFrameCount() << "\n";
    std::cout << "Duration: " << clip.getDurationSeconds() << " seconds\n";
}

void App::reverse() {
    if (!project.hasClip()) {
        throw std::runtime_error("No clip loaded");
    }

    AudioClip edited = AudioProcessor::reverse(project.getClip());
    project.applyEdit(std::move(edited));

    std::cout << "Reversed audio\n";
}

void App::saveCurrent(const std::vector<std::string>& args) {
    if (!project.hasClip()) {
        throw std::runtime_error("No clip loaded");
    }

    if (args.size() != 1) {
        throw std::runtime_error("Usage: save <path>");
    }

    std::filesystem::path path = args[0];

    WavFile::save(project.getClip(), path);

    std::cout << "Saved to: " << path << "\n";
}

void App::undo() {
    if (!project.canUndo()) {
        std::cout << "Nothing to undo\n";
        return;
    }

    project.undo();

    std::cout << "Undo complete\n";
}

void App::redo() {
    if (!project.canRedo()) {
        std::cout << "Nothing to redo\n";
        return;
    }

    project.redo();

    std::cout << "Redo complete\n";
}

void App::trim(const std::vector<std::string>& args) {
    if (!project.hasClip()) {
        throw std::runtime_error("No clip loaded");
    }

    if (args.size() != 2) {
        throw std::runtime_error("Usage: trim <startSeconds> <endSeconds>");
    }

    double start = std::stod(args[0]);
    double end = std::stod(args[1]);

    AudioClip edited = AudioProcessor::trim(project.getClip(), start, end);
    project.applyEdit(std::move(edited));

    std::cout << "Trim complete\n";
}

void App::normalize() {
    if (!project.hasClip()) {
        throw std::runtime_error("No clip loaded");
    }

    AudioClip edited = AudioProcessor::normalize(project.getClip());
    project.applyEdit(std::move(edited));

    std::cout << "Normalized audio\n";
}

void App::renderWaveform(const std::vector<std::string>& args) {
    if (!project.hasClip()) {
        throw std::runtime_error("No clip loaded");
    }

    if (args.size() != 3) {
        throw std::runtime_error("Usage: render <outputPath> <columns> <height>");
    }

    std::filesystem::path outputPath = args[0];
    size_t columns = static_cast<size_t>(std::stoull(args[1]));
    size_t height = static_cast<size_t>(std::stoull(args[2]));

    auto waveform = WaveformGenerator::generate(project.getClip(), columns);

    Renderer::saveWaveformBmp(
        waveform,
        height,
        RgbColor{0, 120, 255},
        outputPath
    );

    std::cout << "Rendered waveform to: " << outputPath << '\n';
}
} // namespace wavtool