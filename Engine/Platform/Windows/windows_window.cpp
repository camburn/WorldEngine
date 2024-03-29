#include "engine.hpp"
#include "windows_window.hpp"

#include <GLFW/glfw3.h>

namespace engine {

static void glfw_error_callback(int error, const char* description) {
    ENGINE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window* Window::create(int width, int height) {
    return new WindowsWindow(width, height);
}

WindowsWindow::WindowsWindow(): width(500), height(500) {
    init("test");
}

WindowsWindow::WindowsWindow(int width, int height) : width(width), height(height) {
    init("test");
}

WindowsWindow::~WindowsWindow() {
    shutdown();
}

void WindowsWindow::init(std::string title) {
    ENGINE_INFO("Creating Windows Window: {2} - ({0}, {1})", width, height, title);
    if (!glfwInit()){
        ENGINE_ERROR("Failed to initialise GLFW in Windows Window");
        return;
    }

    #ifdef OPENGL_COMPATIBILITY
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    #else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    glfwSetErrorCallback(glfw_error_callback);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    //glfwMakeContextCurrent(window);
    context = new enginegl::OpenGLContext(window);
    context->init();

    glfwSetWindowUserPointer(window, this);

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
    
    // Connect the glfw callback to this window object
    glfwSetWindowSizeCallback(window,
        [](GLFWwindow* window, int new_width, int new_height) {
            auto linux_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
            bus::publish(std::make_unique<WindowResize>(new_width, new_height));
            ENGINE_INFO("Window resized {0} x {1}", new_width, new_height);
            linux_window->width = new_width;
            linux_window->height = new_height;
        }
    );
}

void WindowsWindow::shutdown() {
    ENGINE_INFO("Linux Window Shutdown");
    glfwDestroyWindow(window);
    glfwTerminate();
}

void WindowsWindow::on_update() {
    glfwPollEvents();
    context->swap_buffers();
}

unsigned int WindowsWindow::get_width() {
    return width;
}

unsigned int WindowsWindow::get_height() {
    return height;
}

} // namespace
