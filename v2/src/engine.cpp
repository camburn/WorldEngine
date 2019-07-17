#include <iostream>

#include "application.hpp"

int main() {
    std::cout << "Engine Started" << std::endl;
    engine::Application application;
    application.run();
    return 0;
}
