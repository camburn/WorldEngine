#include "engine.hpp"
#include "shader.hpp"

#include "glad/glad.h"

#include "Platform/OpenGL/shader_loader.hpp"
    
namespace engine {

Shader::Shader(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path) {

    shader_id = enginegl::build_program(vertex_shader_file_path, fragment_shader_file_path);
}

Shader::~Shader() {
    glDeleteProgram(shader_id);
}

void Shader::bind() {
    glUseProgram(shader_id);
}

void Shader::unbind() {
    glUseProgram(0);

    #ifdef OPENGL_COMPATIBILITY
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    #endif
}

}  // Namespace