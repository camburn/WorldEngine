#ifndef _SHADER_LOADER_HPP
#define _SHADER_LOADER_HPP
#include <string>
#include <glad/glad.h>

namespace enginegl {

std::string &get_last_error();

GLuint load_shader(std::string &data, GLuint shader_id);

GLuint build_program(std::string vs, std::string fs, std::string gs);

GLuint load_build_program(
    std::string &vertex_shader_file_path,
    std::string &fragment_shader_file_path,
    std::string &vs_data,
    std::string &fs_data
);

GLuint load_build_program(
    std::string& vertex_shader_file_path,
    std::string& geometry_shader_file_path,
    std::string& fragment_shader_file_path,
    std::string& vs_data,
    std::string& gs_data,
    std::string& fs_data
);


} // namespace
#endif
