#include "wavtool/App.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

struct AppRunResult {
    std::string out;
    std::string err;
};

AppRunResult runAppWithInput(const std::string& input) {
    std::istringstream fakeInput(input);
    std::ostringstream fakeOutput;
    std::ostringstream fakeError;

    auto* oldCin = std::cin.rdbuf(fakeInput.rdbuf());
    auto* oldCout = std::cout.rdbuf(fakeOutput.rdbuf());
    auto* oldCerr = std::cerr.rdbuf(fakeError.rdbuf());

    try {
        wavtool::App app;
        app.run();
    } catch (...) {
        std::cin.rdbuf(oldCin);
        std::cout.rdbuf(oldCout);
        std::cerr.rdbuf(oldCerr);
        throw;
    }

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::cerr.rdbuf(oldCerr);

    return AppRunResult{fakeOutput.str(), fakeError.str()};
}

bool contains(const std::string& text, const std::string& target) {
    return text.find(target) != std::string::npos;
}

void removeIfExists(const std::filesystem::path& path) {
    if (std::filesystem::exists(path)) {
        std::filesystem::remove(path);
    }
}

void testAppStartsAndExits() {
    auto result = runAppWithInput("exit\n");

    require(contains(result.out, "WavTool CLI"), "App should print startup banner");
    require(contains(result.out, "Type 'help' for commands."), "App should print help hint");
    require(result.err.empty(), "exit should not print error");
}

void testHelpCommand() {
    auto result = runAppWithInput("help\nexit\n");

    require(contains(result.out, "Commands:"), "help should print command list");
    require(contains(result.out, "load <path>"), "help should include load");
    require(contains(result.out, "save <path>"), "help should include save");
    require(contains(result.out, "render <output> <columns> <height>"), "help should include render");
    require(result.err.empty(), "help should not print error");
}

void testUnknownCommand() {
    auto result = runAppWithInput("banana\nexit\n");

    require(contains(result.out, "Unknown command"), "Unknown command should print message");
    require(result.err.empty(), "Unknown command should not be fatal error");
}

void testInfoWithoutClip() {
    auto result = runAppWithInput("info\nexit\n");

    require(contains(result.out, "No clip loaded"), "info without clip should say no clip loaded");
}

void testLoadAndInfo() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "info\n"
        "exit\n"
    );

    require(contains(result.out, "Loaded:"), "load should print loaded message");
    require(contains(result.out, "Name:"), "info should print clip name");
    require(contains(result.out, "Channels:"), "info should print channels");
    require(contains(result.out, "Sample Rate:"), "info should print sample rate");
    require(contains(result.out, "Bit Depth:"), "info should print bit depth");
    require(contains(result.out, "Frames:"), "info should print frame count");
    require(contains(result.out, "Duration:"), "info should print duration");
    require(result.err.empty(), "valid load/info should not print error");
}

void testLoadBadPathPrintsError() {
    auto result = runAppWithInput(
        "load test_assets/does_not_exist.wav\n"
        "exit\n"
    );

    require(contains(result.err, "Error:"), "bad load path should print error");
}

void testLoadWrongArgCount() {
    auto result = runAppWithInput(
        "load\n"
        "exit\n"
    );

    require(contains(result.err, "Usage: load <path>"), "load without path should print usage");
}

void testReverseRequiresClip() {
    auto result = runAppWithInput(
        "reverse\n"
        "exit\n"
    );

    require(contains(result.err, "No clip loaded"), "reverse without clip should fail");
}

void testReverseUndoRedoFlow() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "reverse\n"
        "undo\n"
        "redo\n"
        "exit\n"
    );

    require(contains(result.out, "Reversed audio"), "reverse should succeed");
    require(contains(result.out, "Undo complete"), "undo should succeed after reverse");
    require(contains(result.out, "Redo complete"), "redo should succeed after undo");
    require(result.err.empty(), "reverse undo redo should not print errors");
}

void testUndoRedoWhenUnavailable() {
    auto result = runAppWithInput(
        "undo\n"
        "redo\n"
        "exit\n"
    );

    require(contains(result.out, "Nothing to undo"), "undo with empty history should be friendly");
    require(contains(result.out, "Nothing to redo"), "redo with empty history should be friendly");
    require(result.err.empty(), "empty undo/redo should not print errors");
}

void testSaveRequiresClip() {
    auto result = runAppWithInput(
        "save test_output/app_should_not_save.wav\n"
        "exit\n"
    );

    require(contains(result.err, "No clip loaded"), "save without clip should fail");
}

void testSaveWrongArgCount() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "save\n"
        "exit\n"
    );

    require(contains(result.err, "Usage: save <path>"), "save without path should print usage");
}

void testSaveCreatesOutputFile() {
    std::filesystem::create_directories("test_output");
    std::filesystem::path outputPath = "test_output/app_save_test.wav";
    removeIfExists(outputPath);

    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "reverse\n"
        "save test_output/app_save_test.wav\n"
        "exit\n"
    );

    require(contains(result.out, "Saved to:"), "save should print saved message");
    require(std::filesystem::exists(outputPath), "save should create output WAV");
    require(std::filesystem::file_size(outputPath) > 44, "saved WAV should be larger than header");

    removeIfExists(outputPath);
}

void testTrimRequiresClip() {
    auto result = runAppWithInput(
        "trim 1 2\n"
        "exit\n"
    );

    require(contains(result.err, "No clip loaded"), "trim without clip should fail");
}

void testTrimWrongArgCount() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "trim 1\n"
        "exit\n"
    );

    require(contains(result.err, "Usage: trim <startSeconds> <endSeconds>"),
            "trim wrong arg count should print usage");
}

void testTrimInvalidTimeRange() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "trim 5 2\n"
        "exit\n"
    );

    require(contains(result.err, "Error:"), "invalid trim range should print error");
}

void testTrimValidRange() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "trim 0 1\n"
        "info\n"
        "exit\n"
    );

    require(contains(result.out, "Trim complete"), "valid trim should succeed");
    require(contains(result.out, "Duration:"), "info after trim should work");
    require(result.err.empty(), "valid trim should not print error");
}

void testNormalizeRequiresClip() {
    auto result = runAppWithInput(
        "normalize\n"
        "exit\n"
    );

    require(contains(result.err, "No clip loaded"), "normalize without clip should fail");
}

void testNormalizeValidClip() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "normalize\n"
        "undo\n"
        "redo\n"
        "exit\n"
    );

    require(contains(result.out, "Normalized audio"), "normalize should succeed");
    require(contains(result.out, "Undo complete"), "undo should work after normalize");
    require(contains(result.out, "Redo complete"), "redo should work after normalize");
    require(result.err.empty(), "normalize flow should not print error");
}

void testRenderRequiresClip() {
    auto result = runAppWithInput(
        "render test_output/app_render_test.bmp 100 100\n"
        "exit\n"
    );

    require(contains(result.err, "No clip loaded"), "render without clip should fail");
}

void testRenderWrongArgCount() {
    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "render test_output/app_render_test.bmp 100\n"
        "exit\n"
    );

    require(contains(result.err, "Usage: render <outputPath> <columns> <height>"),
            "render wrong arg count should print usage");
}

void testRenderCreatesBmpFile() {
    std::filesystem::create_directories("test_output");
    std::filesystem::path outputPath = "test_output/app_render_test.bmp";
    removeIfExists(outputPath);

    auto result = runAppWithInput(
        "load test_assets/tokyo-rain.wav\n"
        "render test_output/app_render_test.bmp 300 200\n"
        "exit\n"
    );

    require(contains(result.out, "Rendered waveform to:"), "render should print success message");
    require(std::filesystem::exists(outputPath), "render should create BMP file");
    require(std::filesystem::file_size(outputPath) > 54, "BMP should be larger than header");

    std::ifstream file(outputPath, std::ios::binary);
    require(static_cast<bool>(file), "should be able to open rendered BMP");

    char b = 0;
    char m = 0;
    file.get(b);
    file.get(m);

    require(b == 'B', "BMP first byte should be B");
    require(m == 'M', "BMP second byte should be M");

    file.close();

    removeIfExists(outputPath);
}

void testCommandSequenceFullWorkflow() {
    std::filesystem::create_directories("test_output");

    std::filesystem::path wavOutput = "test_output/app_full_workflow.wav";
    std::filesystem::path bmpOutput = "test_output/app_full_workflow.bmp";

    removeIfExists(wavOutput);
    removeIfExists(bmpOutput);

    auto result = runAppWithInput(
        "help\n"
        "load test_assets/tokyo-rain.wav\n"
        "info\n"
        "reverse\n"
        "undo\n"
        "redo\n"
        "trim 0 1\n"
        "normalize\n"
        "render test_output/app_full_workflow.bmp 300 200\n"
        "save test_output/app_full_workflow.wav\n"
        "exit\n"
    );

    require(contains(result.out, "Commands:"), "workflow should show help");
    require(contains(result.out, "Loaded:"), "workflow should load");
    require(contains(result.out, "Reversed audio"), "workflow should reverse");
    require(contains(result.out, "Undo complete"), "workflow should undo");
    require(contains(result.out, "Redo complete"), "workflow should redo");
    require(contains(result.out, "Trim complete"), "workflow should trim");
    require(contains(result.out, "Normalized audio"), "workflow should normalize");
    require(contains(result.out, "Rendered waveform to:"), "workflow should render");
    require(contains(result.out, "Saved to:"), "workflow should save");

    require(std::filesystem::exists(wavOutput), "workflow should create WAV output");
    require(std::filesystem::exists(bmpOutput), "workflow should create BMP output");

    require(std::filesystem::file_size(wavOutput) > 44, "workflow WAV output should be valid-sized");
    require(std::filesystem::file_size(bmpOutput) > 54, "workflow BMP output should be valid-sized");

    require(result.err.empty(), "full workflow should not print errors");

    removeIfExists(wavOutput);
    removeIfExists(bmpOutput);
}

} // namespace

int main() {
    try {
        testAppStartsAndExits();
        testHelpCommand();
        testUnknownCommand();

        testInfoWithoutClip();

        testLoadAndInfo();
        testLoadBadPathPrintsError();
        testLoadWrongArgCount();

        testReverseRequiresClip();
        testReverseUndoRedoFlow();

        testUndoRedoWhenUnavailable();

        testSaveRequiresClip();
        testSaveWrongArgCount();
        testSaveCreatesOutputFile();

        testTrimRequiresClip();
        testTrimWrongArgCount();
        testTrimInvalidTimeRange();
        testTrimValidRange();

        testNormalizeRequiresClip();
        testNormalizeValidClip();

        testRenderRequiresClip();
        testRenderWrongArgCount();
        testRenderCreatesBmpFile();

        testCommandSequenceFullWorkflow();

        std::cout << "All App tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "App test failed: " << e.what() << '\n';
        return 1;
    }
}