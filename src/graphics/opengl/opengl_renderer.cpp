#include "graphics/opengl/gl_renderer.hpp"

namespace opengl {

int width = 1920;
int height = 1080;
GLFWwindow* window;

void init() {
    if (!glfwInit()) {
        std::cout << "Failed to initialise GLFW" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //Disable legacy OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
   
    //Debugging 
    std::cout << "DEBUGGING ON" << std::endl;
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

}

void create_window() {
    window = glfwCreateWindow(width, height, "Embedded Python", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Failed to initialise GLEW" << std::endl;
        std::cout << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        exit(1);
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}


GLFWwindow* get_window() {
    return window;
}

} // end namespace
