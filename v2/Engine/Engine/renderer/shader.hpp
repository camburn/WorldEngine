#ifndef _SHADER_HPP
#define _SHADER_HPP
#include <string>
#include <map>
#include <set>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"

namespace engine {

class Shader {
public:
    Shader(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path);
    ~Shader();
    void bind();
    void unbind();
    bool recompile();

    uint32_t get_id() { return shader_id; }

    void on_ui_render(bool draw);

    void register_vertex_array(std::shared_ptr<VertexArray> vao);
    bool is_vertex_array_registered(std::shared_ptr<VertexArray> vao);

    void upload_u_mat4(const std::string& u_name, const glm::mat4& matrix);
    void upload_u_vec4(const std::string& u_name, const glm::vec4& vec);
    void upload_u_vec3(const std::string& u_name, const glm::vec3& vec);
    void upload_u_int1(const std::string& u_name, const GLint& value);

    BufferLayout attribute_layout();
    bool attribute_supported(std::string name) {
        name = GLTF_TO_SHADER_ATTRIBUTE[name];
        for (auto & [index, attribute]: attributes) {
            if (attribute.name == name) {
                return true;
            }
        }
        return false;
    }

    int attribute_location(std::string name) {
        name = GLTF_TO_SHADER_ATTRIBUTE[name];
        for (auto & [index, attribute]: attributes) {
            if (attribute.name == name) {
                return attribute.index;
            }
        }
        return -1;
    }

    bool uniform_supported(std::string name) {
        if (uniforms.count(name) > 0) {
            return true;
        }
        return false;
    }

    int uniform_texture_unit(std::string name) {
        if (uniforms.count(name) > 0) {
            return uniforms[name].texture_unit;
        }
        return -1;
    }

    std::string attribute_name_normalise(std::string name) {
        return GLTF_TO_SHADER_ATTRIBUTE[name];
    }

private:
    std::map<std::string, std::string> GLTF_TO_SHADER_ATTRIBUTE {
        {"POSITION", "position"},
        {"NORMAL", "normal"},
        {"TEXCOORD_0", "texcoord"},
    };
    
    struct Uniform {
        Uniform() {}
        Uniform(GLint index, std::string name, GLenum type, GLfloat size):
            index(index), name(name), type(type), size(size), texture_unit(-1)  {}
        Uniform(GLint index, std::string name, GLenum type, GLfloat size, GLint texture_unit):
            index(index), name(name), type(type), size(size), texture_unit(texture_unit) {}
        GLint index;
        std::string name;
        GLenum type;
        GLfloat size;
        GLint texture_unit;
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
    std::map<std::string, Uniform> uniforms;
    std::map<int, Attribute> attributes;
    std::set<unsigned int> registered_vertex_arrays;
    std::string vs_shader_path;
    std::string fs_shader_path;
    std::string fs_data;
    std::string vs_data;

    GLuint build();

    void inspect_uniforms();
    void inspect_attributes();
    void program_resources();
};

}  // namespace

#endif