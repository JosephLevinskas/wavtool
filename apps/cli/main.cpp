#include "wavtool/App.h"

#include <exception>
#include <iostream>

int main() {
    try {
        wavtool::App app;
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
}