#include "engine.hpp"
#include "linux_window.hpp"

#include <GLFW/glfw3.h>

namespace engine {

static void glfw_error_callback(int error, const char* description) {
    ENGINE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window* Window::create() {
    return new LinuxWindow();
}

LinuxWindow::LinuxWindow(): width(500), height(500) {
    init("test");
}

LinuxWindow::~LinuxWindow() {
    shutdown();
}

void LinuxWindow::init(std::string title) {
    ENGINE_INFO("Creating Linux Window: {2} - ({0}, {1})", width, height, title);
    if (!glfwInit()){
        ENGINE_ERROR("Failed to initialise GLFW in Linux Window");
        return;
    }

    #ifndef OPENGL_COMPATIBILITY
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    glfwSetErrorCallback(glfw_error_callback);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    //glfwMakeContextCurrent(window);
    context = new enginegl::OpenGLContext(window);
    context->init();

    glfwSetWindowCloseCallback(window, 
        [](GLFWwindow* window){
            ENGINE_INFO("Window Close Callback");
            bus::publish(std::make_unique<ApplicationEvent>(QUIT));
        }
    );

    glfwSetKeyCallback(window, 
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            ENGINE_TRACE("Key Pressed {0}, {1}", key, action);
            bus::publish(std::make_unique<KeyEvent>(key, action));
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
    context->swap_buffers();
}

unsigned int LinuxWindow::get_width() {
    return width;
}

unsigned int LinuxWindow::get_height() {
    return height;
}

} // namespace
