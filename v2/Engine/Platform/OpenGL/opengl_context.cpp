#include "engine.hpp"
#include "opengl_context.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace enginegl {

OpenGLContext::OpenGLContext(GLFWwindow* window_handle): window_handle(window_handle) {
    ENGINE_ASSERT(window_handle, "Window handle is null");
    ENGINE_INFO("Context created");
}

void OpenGLContext::init() {
    glfwMakeContextCurrent(window_handle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    ENGINE_ASSERT(status, "GLAD Failed to initialise");

    ENGINE_INFO("OpenGL Info:");
    ENGINE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    ENGINE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    ENGINE_INFO("  Version: {0}", glGetString(GL_VERSION));
}

void OpenGLContext::swap_buffers() {
    glfwSwapBuffers(window_handle);
}

} //namespace