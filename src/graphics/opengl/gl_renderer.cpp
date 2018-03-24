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

void set_base_state() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, width, height);
}

GLFWwindow* get_window() {
    return window;
}

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam) {
    // ignore non-significant error/warning codes
    // Message 131184 displays Buffer memory info,
    // TODO: Load this data into a debug message window
    if (id == 131184) return;
    if (id == 131185) return; // This details VBO allocations (and size)
    if (id == 131204) return; // Texture state usage warning: Texture 0 is base level inconsistent. Check texture size.
    // if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    // MESA Specific debug message, does not have a fixed id number
    if (source == GL_DEBUG_SOURCE_SHADER_COMPILER && severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "OpenGL Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "---------------" << std::endl;
}

void enable_debug() {
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

GLuint LoadShader(const char* file_path, GLuint ShaderID) {   
    GLint Result = GL_FALSE;
    int InfoLogLength;

    char* vertex_src = loadFile(file_path);
    // Compile the Shader
    fprintf(stdout, "Compiling Shader: %s\n", file_path);
    glShaderSource(ShaderID, 1, &vertex_src, NULL);
    glCompileShader(ShaderID);
    // Check result
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ShaderErrorMessage = (char *)malloc(InfoLogLength);
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, ShaderErrorMessage);
        fprintf(stdout, "Shader Source\n\n%s\n\n", vertex_src);
        fprintf(stderr, "Shader Compile Failed:\n %s\n", ShaderErrorMessage);
        free(ShaderErrorMessage);
        throw std::runtime_error("Failed to compile shader");
    }
    return ShaderID;
}

GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path) {
    GLuint vertexShaderID = LoadShader(vertex_file_path, glCreateShader(GL_VERTEX_SHADER));
    GLuint fragmentShaderID = LoadShader(fragment_file_path, glCreateShader(GL_FRAGMENT_SHADER));
    GLuint programID = glCreateProgram();
    GLint Result = GL_FALSE;
    int InfoLogLength;
    fprintf(stdout, "Linking shaders to program\n");
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &Result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ProgramErrorMessage = (char *)malloc(InfoLogLength);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage);
        fprintf(stderr, "Program Link Failure:\n %s\n", ProgramErrorMessage);
        free(ProgramErrorMessage);
        throw std::runtime_error("Failed to link shaders to program");
    }
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    return programID;
}

GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path) {
    GLuint vertexShaderID = LoadShader(vertex_file_path, glCreateShader(GL_VERTEX_SHADER));
    GLuint fragmentShaderID = LoadShader(fragment_file_path, glCreateShader(GL_FRAGMENT_SHADER));
    GLuint geometryShaderID = LoadShader(geometry_file_path, glCreateShader(GL_GEOMETRY_SHADER));
    GLuint programID = glCreateProgram();
    GLint Result = GL_FALSE;
    int InfoLogLength;
    fprintf(stdout, "Linking shaders to program\n");
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glAttachShader(programID, geometryShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &Result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ProgramErrorMessage = (char *)malloc(InfoLogLength);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage);
        fprintf(stderr, "Program Link Failure:\n %s\n", ProgramErrorMessage);
        free(ProgramErrorMessage);
        throw std::runtime_error("Failed to link shaders to program");
    }
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDetachShader(programID, geometryShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    return programID;
}

} // end namespace
