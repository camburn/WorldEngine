#include "engine.hpp"
#include "Engine/platform/linux_window.hpp"

#include <GLFW/glfw3.h>

namespace engine {

static void glfw_error_callback(int error, const char* description) {
    ENGINE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window* Window::create() {
    return new LinuxWindow();
}

LinuxWindow::LinuxWindow() {
    ENGINE_INFO("Linux Window Created");
    init(500, 500, "test");
}

LinuxWindow::~LinuxWindow() {
    shutdown();
}

void LinuxWindow::init(int width, int height, std::string title) {
    if (!glfwInit()){
        ENGINE_ERROR("Failed to initialise GLFW in Linux Window");
    }

    glfwSetErrorCallback(glfw_error_callback);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(window);

    glfwSetWindowCloseCallback(window, 
        [](GLFWwindow* window){
            //WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            //WindowCloseEvent event;
            //data.EventCallback(event);
            ENGINE_INFO("Window Close Callback");
            bus::publish(std::make_unique<ApplicationEvent>(QUIT));
        }
    );

}

void LinuxWindow::shutdown() {
    ENGINE_INFO("Linux Window Shutdown");
    glfwDestroyWindow(window);
    glfwTerminate();
}

void LinuxWindow::on_update() {
    glfwPollEvents();
    glfwSwapBuffers(window);
}

unsigned int LinuxWindow::get_width() {
    return 0;
}

unsigned int LinuxWindow::get_height() {
    return 0;
}

} // namespace
