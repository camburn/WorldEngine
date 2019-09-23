#include "engine.hpp"
#include "shader_loader.hpp"
#include "Tools/io.hpp"

namespace enginegl {

static std::string last_error;

std::string &get_last_error() {
    return last_error;
}

void CheckMessage(GLuint program_id) {
    int log_length;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        char* err_msg = (char *)malloc(log_length);
        glGetProgramInfoLog(program_id, log_length, NULL, err_msg);
        fprintf(stderr, "Program Link Failure:\n %s\n", err_msg);
        ENGINE_ERROR("Program link error", err_msg);
        free(err_msg);
    }
}

GLuint load_shader(std::string &source, GLuint shader_id) {

    const char* c_source = source.c_str();
    glShaderSource(shader_id, 1, &c_source, NULL);
    glCompileShader(shader_id);

    GLint result = GL_FALSE;
    int log_length;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        char* err_msg = (char *)malloc(log_length);
        glGetShaderInfoLog(shader_id, log_length, NULL, err_msg);
        ENGINE_TRACE("Shader Source\n{0}", source);
        ENGINE_ERROR("Shader Compile Failed {0}", err_msg);
        last_error = err_msg;
        free(err_msg);
        return 0;
    }
    return shader_id;
}

GLuint load_build_program(
    std::string &vertex_shader_file_path,
    std::string &fragment_shader_file_path,
    std::string &vs_data,
    std::string &fs_data) {

    vs_data = read_file(vertex_shader_file_path);
    fs_data = read_file(fragment_shader_file_path);

    return build_program(vs_data, fs_data);
}

GLuint build_program(std::string vs, std::string fs) {

    ENGINE_DEBUG("Building Program");

    GLuint vertex_shader_id = load_shader(vs, glCreateShader(GL_VERTEX_SHADER));
    GLuint fragment_shader_id = load_shader(fs, glCreateShader(GL_FRAGMENT_SHADER));

    if (!vertex_shader_id || !fragment_shader_id) {
        return 0;
    }

    GLuint program_id = glCreateProgram();

    GLint result = GL_FALSE;
    GLint valid = 1;
    int log_length;
    ENGINE_DEBUG("Linking shaders to program");

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    if (!result) {
        CheckMessage(program_id);
        ENGINE_ERROR("Shader link failed");
        return 0;
    }

    // TODO: Validation checks current state, it should be called only on debug
    // just before a using the shader
    // In this case having to seperate samplers (sampler2D + samplerCube) which
    // by default are all bound to texture unit 0 will fail. It requries setup
    // of texture units first.

    //glValidateProgram(program_id);
    //glGetProgramiv(program_id, GL_VALIDATE_STATUS, &valid);
    if (!valid) {
        CheckMessage(program_id);
        ENGINE_ERROR("Shader failed validation");
        return 0;
    }
    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
    return program_id;
}

} // namespace
