#include "engine.hpp"
#include "shader.hpp"

#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>

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

void Shader::upload_u_mat4(const std::string& u_name, const glm::mat4& matrix) {
    GLint location = glGetUniformLocation(shader_id, u_name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}  // Namespace