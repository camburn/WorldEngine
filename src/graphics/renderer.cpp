#include "graphics/renderer.hpp"

Renderer::Renderer() {
    // Prepare this renderer for rendering!
    opengl::init();
    opengl::create_window();
    opengl::enable_debug();
}

// Who really cares about a shader?

void Renderer::LoadShaders() {

};

GLFWwindow* Renderer::get_window() {
    return opengl::get_window();
}
