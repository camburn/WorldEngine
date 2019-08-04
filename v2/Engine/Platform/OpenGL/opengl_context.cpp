#include "engine.hpp"
#include "opengl_context.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace enginegl {

std::map<GLenum, std::string> gl_error_types = {
    { GL_DEBUG_TYPE_ERROR, "GL_DEBUG_TYPE_ERROR" },
    { GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR" },
    { GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR" },
    { GL_DEBUG_TYPE_PORTABILITY, "GL_DEBUG_TYPE_PORTABILITY" },
    { GL_DEBUG_TYPE_PERFORMANCE, "GL_DEBUG_TYPE_PERFORMANCE" },
    { GL_DEBUG_TYPE_MARKER, "GL_DEBUG_TYPE_MARKER" },
    { GL_DEBUG_TYPE_PUSH_GROUP, "GL_DEBUG_TYPE_PUSH_GROUP" },
    { GL_DEBUG_TYPE_POP_GROUP, "GL_DEBUG_TYPE_POP_GROUP" },
    { GL_DEBUG_TYPE_OTHER, "GL_DEBUG_TYPE_OTHER" }
};

void GLAPIENTRY
MessageCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam
    ) {
    std::string error_type_str = gl_error_types[type];
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        ENGINE_DEBUG("{0} : {1}", error_type_str, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        ENGINE_INFO("{0} : {1}", error_type_str, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        ENGINE_WARN("{0} : {1}", error_type_str, message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        ENGINE_ERROR("{0} : {1}", error_type_str, message);
        break;
    default:
        break;
    }
}

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

    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
    
}

void OpenGLContext::swap_buffers() {
    glfwSwapBuffers(window_handle);
}

} //namespace