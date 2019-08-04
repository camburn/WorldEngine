#ifndef _SHADER_HPP
#define _SHADER_HPP
#include <string>

#include "glad/glad.h"

#include <glm/glm.hpp>

namespace engine {

class Shader {
public:
    Shader(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path);
    ~Shader();
    void bind();
    void unbind();
    void recompile();

    void on_ui_render(bool draw);

    void upload_u_mat4(const std::string& u_name, const glm::mat4& matrix);
    void upload_u_vec4(const std::string& u_name, const glm::vec4& vec);

private:
    struct Uniform {
        Uniform() {}
        Uniform(GLint index, std::string name, GLenum type, GLfloat size):
            index(index), name(name), type(type), size(size) {}
        GLint index;
        std::string name;
        GLenum type;
        GLfloat size;
    };

    struct Attribute {
        Attribute() {}
        Attribute(GLint index, std::string name, GLenum type, GLfloat size):
            index(index), name(name), type(type), size(size) {}
        GLint index;
        std::string name;
        GLenum type;
        GLfloat size;
    };

    uint32_t shader_id;
    std::unordered_map<std::string, Uniform> uniforms;
    std::unordered_map<std::string, Attribute> attributes;
    std::string vs_shader_path;
    std::string fs_shader_path;

    GLuint build();

    void inspect_uniforms();
    void inspect_attributes();
    void program_resources();
};

}  // namespace

#endif