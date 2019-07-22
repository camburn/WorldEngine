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

private:
    void init(int width, int height, std::string title);
    void shutdown();

    GLFWwindow* window;
};

}
#endif
