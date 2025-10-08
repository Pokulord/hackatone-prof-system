#include "Application.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}