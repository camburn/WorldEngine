#ifndef _SHADER_LOADER_HPP
#define _SHADER_LOADER_HPP
#include <string>
#include <glad/glad.h>

namespace enginegl {

GLuint load_shader(std::string &data, GLuint shader_id);

GLuint build_program(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path);

} // namespace
#endif
