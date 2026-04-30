#include "wavtool/App.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace wavtool {

void App::run() {
    std::cout << "WavTool CLI\n";
    std::cout << "Type 'help' for commands.\n";

    std::string line;

    while (true) {
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

    if (command.empty()) {
        return;
    }

    if (command == "exit" || command == "quit") {
        throw std::runtime_error("Exit requested");
    }

    std::cout << "Command received: " << command << '\n';
}

} // namespace wavtool