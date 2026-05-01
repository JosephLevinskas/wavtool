#pragma once
#include "wavtool/Project.h"

namespace wavtool {

class App {
public:
    void run();

private:
    Project project;

    bool running = true;

    void printHelp() const;
    void handleCommand(const std::string& line);

    void loadFile(const std::vector<std::string>& args);
    void saveCurrent(const std::vector<std::string>& args);
    void reverse();
    void trim(const std::vector<std::string>& args);
    void normalize();
    void renderWaveform(const std::vector<std::string>& args);
    void undo();
    void redo();
    void printCurrentInfo() const;
};

} 