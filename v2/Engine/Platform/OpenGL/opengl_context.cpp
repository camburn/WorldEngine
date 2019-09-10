#include "engine.hpp"
#include "opengl_context.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace enginegl {

const std::map<GLenum, std::string> gl_error_types = {
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

const std::map<GLenum, std::string> gl_source_types = {
    { GL_DEBUG_SOURCE_API, "GL_DEBUG_SOURCE_API" },
    { GL_DEBUG_SOURCE_WINDOW_SYSTEM, "GL_DEBUG_SOURCE_WINDOW_SYSTEM" },
    { GL_DEBUG_SOURCE_SHADER_COMPILER, "GL_DEBUG_SOURCE_SHADER_COMPILER" },
    { GL_DEBUG_SOURCE_THIRD_PARTY, "GL_DEBUG_SOURCE_THIRD_PARTY" },
    { GL_DEBUG_SOURCE_APPLICATION, "GL_DEBUG_SOURCE_APPLICATION" },
    { GL_DEBUG_SOURCE_OTHER , "GL_DEBUG_SOURCE_OTHER" }
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
    std::string error_type_str = gl_error_types.at(type);
    std::string source_str = gl_source_types.at(source);
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        ENGINE_DEBUG("({0} - {1}): {2}", source_str, error_type_str, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        ENGINE_INFO("({0} - {1}): {2}", source_str, error_type_str, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        ENGINE_WARN("({0} - {1}): {2}", source_str, error_type_str, message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        ENGINE_ERROR("({0} - {1}): {2}", source_str, error_type_str, message);
        break;
    default:
        break;
    }
}

void _check_gl_error(const char *file, int line) {
    GLenum err (glGetError());

    while(err!=GL_NO_ERROR) {
        std::string error;

        switch(err) {
            case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        ENGINE_ERROR("OpenGL: {0} - {1}:{0}", error.c_str(), file, line);
        err=glGetError();
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
    #ifdef ENGINE_DEBUG_ENABLED
    glEnable( GL_DEBUG_OUTPUT );
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //Filter out Nvidia Buffer detailed info
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glDebugMessageCallback( MessageCallback, 0 );
    #endif
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLContext::swap_buffers() {
    glfwSwapBuffers(window_handle);
}

} //namespace