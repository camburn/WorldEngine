#include "graphics/opengl/gl_renderer.hpp"

namespace opengl {

GLuint shadow_map_buffer_id;
GLuint shadow_map_texture_id;
GLuint shadow_cube_buffer_id;
GLuint shadow_cube_texture_id;
GLuint ssbo_id;
GLuint depth_map_width = 4096;
GLuint depth_map_height = 4096;
int width = 1920;
int height = 1080;
GLFWwindow* window;
bool blit_stall_message = false;

void init() {
    if (!glfwInit()) {
        std::cout << "Failed to initialise GLFW" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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

void clear_buffers() {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glCullFace(GL_BACK);
}

void create_common_buffers() {
    TextureBuffer shadow_map_texture_data = DepthMapBuffer(depth_map_width, depth_map_height);
    shadow_map_texture_id = shadow_map_texture_data.texture_id;
    shadow_map_buffer_id = shadow_map_texture_data.framebuffer;
    TextureBuffer texture_data = DepthCubeMapBuffer(1024, 1024);
    shadow_cube_buffer_id = texture_data.framebuffer;
    shadow_cube_texture_id = texture_data.texture_id;
    ssbo_id = create_ssbo();
}

void update_uniforms(float uniform_data[], int uniform_size) {
    update_ssbo(uniform_data, uniform_size);
}

void update_uniforms(SharedState &state) {
    update_ssbo(state);
}

void activate_common_buffers() {
    glViewport(0, 0, depth_map_width, depth_map_height);
    // /glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_buffer_id);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glCullFace(GL_FRONT);
    activate_ssbo();
}

void activate_buffer_cube_shadow_map() {
    glViewport(0, 0, 1024, 1024);
    //glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_cube_buffer_id);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glCullFace(GL_FRONT);
}

SharedState output;
void finish_frame() {
    read_ssbo(&output);
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

    if (id == 38) {
        std::cout << "tester";
    }
    // TODO: Fix these warnings
    // Intel Warning to ignore
    if (id == 14) {return;} // OpenGL Debug message (14): Copying to larger program cache: 16 kB -> 32 kB
    if (id == 41) {return;} // Recompiling fragment shader for program 3
    if (id == 42) {return;} // multisampled FBO 0->1
    if (id == 47) {return;} // FS compile took 6.315 ms and stalled the GPU

    // FIXME: this requires fixing
    if (id == 34) {return;} // Using a blit copy to avoid stalling on glBufferSubData(64, 128) (0kb) to a busy (0-65536) / valid (0-192) buffer object.
    if (id == 26) {return;} // As above

    // TODO: This error message needs to be investigated in Intel GPUs
    if (blit_stall_message) { return; }
    if (id == 53) { blit_stall_message = true; }

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

void CheckMessage(GLuint program_id) {
    int InfoLogLength;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ProgramErrorMessage = (char *)malloc(InfoLogLength);
        glGetProgramInfoLog(program_id, InfoLogLength, NULL, ProgramErrorMessage);
        fprintf(stderr, "Program Link Failure:\n %s\n", ProgramErrorMessage);
        free(ProgramErrorMessage);
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
    GLint valid=1;

    fprintf(stdout, "Linking shaders to program\n");
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &Result);
    if (!Result) {
        CheckMessage(programID);
        throw std::runtime_error("Failed to link shaders");
    }

    glUseProgram(programID);
    // Set sampler units
    // TODO: This should happen in a smarter way
    GLint texture_sampler_id = glGetUniformLocation(programID, "texture_diffuse1");
    GLint shadow_sampler_id = glGetUniformLocation(programID, "shadow_map");
    GLint shadow_cube_sampler_id = glGetUniformLocation(programID, "shadow_cube_map");

    if (texture_sampler_id > 0) {
        glUniform1i(texture_sampler_id, 0);
    }
    if (shadow_sampler_id > 0) {
        glUniform1i(shadow_sampler_id, 1);
    }
    if (shadow_cube_sampler_id > 0) {
        glUniform1i(shadow_cube_sampler_id, 2);
    }

    glValidateProgram(programID);
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &valid);
    if (!valid) {
        CheckMessage(programID);
        throw std::runtime_error("Failed to validate program");
    }

    glUseProgram(0);
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
    GLint valid=1;

    fprintf(stdout, "Linking shaders to program\n");
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glAttachShader(programID, geometryShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &Result);
    if (!Result) {
        CheckMessage(programID);
        throw std::runtime_error("Failed to link shaders");
    }

    glUseProgram(programID);
    // Set sampler units

    glValidateProgram(programID);
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &valid);
    if (!valid) {
        CheckMessage(programID);
        throw std::runtime_error("Failed to validate program");
    }

    glUseProgram(0);

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDetachShader(programID, geometryShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    return programID;
}

void draw_ssbo_state(bool* p_open) {
    if (!ImGui::Begin("SSBO State", p_open))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("SSBO State");
    //SharedState output

    ImGui::Text("Point Shadows: %u", output.point_light_count);

    ImGui::Text("Direction Shadow : %u", output.pcf_samples);
    ImGui::Text("Direction map bias : %f", output.shadow_map_bias);
    ImGui::Text("Direction Shadow Enabled: %u", output.use_direction_shadow);
    ImGui::Text("Direction Diffuse: r:%f g:%f b:%f",
        output.direction_light.diffuse.x,
        output.direction_light.diffuse.y,
        output.direction_light.diffuse.z
    );
    ImGui::Text("Direction Ambient: r:%f g:%f b:%f",
        output.direction_light.ambient.x,
        output.direction_light.ambient.y,
        output.direction_light.ambient.z
    );

    ImGui::Text("Point Shadows: %u", output.use_point_shadows);

    ImGui::Text("Debug Normals: %u", output.debug_draw_normals);
    ImGui::Text("Debug Lighting: %u", output.debug_draw_lighting);
    ImGui::Text("Debug TexCoords: %u", output.debug_draw_texcoords);

    ImGui::Text("Padding 0: %i", output._padding_0);
    ImGui::Text("Padding 1: %i", output._padding_1);
    // ImGui::Text("Padding 2: %i", output._padding_2);

    ImGui::End();
}

void draw_buffers(bool* p_open) {
    if (!ImGui::Begin("Planes", p_open))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Plane Data");
    if (ImGui::TreeNode("Shadow Map Buffer")) {
        glActiveTexture(GL_TEXTURE1);
        ImGui::Image((void*)(shadow_map_texture_id), ImVec2(512, 512), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Cube Map Buffer")) {
        glActiveTexture(GL_TEXTURE2);
        //ImGui::Image((void*)(shadow_cube_texture_id), ImVec2(512, 512), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::TreePop();
    }
    ImGui::End();
    glActiveTexture(GL_TEXTURE0);
}

void bind_depth_map () {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_map_texture_id);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_cube_texture_id);
    glActiveTexture(GL_TEXTURE0);
}

} // end namespace
