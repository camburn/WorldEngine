#ifndef _LINUX_WINDOW_HPP
#define _LINUX_WINDOW_HPP
#include <string>

#include <GLFW/glfw3.h>

#include "Engine/window.hpp"
#include "Engine/event/bus.hpp"
#include "Engine/event/event.hpp"

namespace engine {

class LinuxWindow: public Window {

public:
    LinuxWindow();
    ~LinuxWindow();

    void on_update() override;

    unsigned int get_width() override;
    unsigned int get_height() override;
    const std::string name = "Linux Window";

    void* get_native_window() const {return window;}

private:
    void init(std::string title);
    void shutdown();

    GLFWwindow* window;
    int width;
    int height;
};

}
#endif
