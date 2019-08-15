#ifndef _SHADER_HPP
#define _SHADER_HPP
#include <string>
#include <map>
#include <set>

#include "glad/glad.h"

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

    void on_ui_render(bool draw);

    void register_vertex_array(std::shared_ptr<VertexArray> vao);
    bool is_vertex_array_registered(std::shared_ptr<VertexArray> vao);

    void upload_u_mat4(const std::string& u_name, const glm::mat4& matrix);
    void upload_u_vec4(const std::string& u_name, const glm::vec4& vec);
    void upload_u_vec3(const std::string& u_name, const glm::vec3& vec);

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

    bool attribute_location(std::string name) {
        name = GLTF_TO_SHADER_ATTRIBUTE[name];
        for (auto & [index, attribute]: attributes) {
            if (attribute.name == name) {
                return attribute.index;
            }
        }
        return -1;
    }

    std::string attribute_name_normalise(std::string name) {
        return GLTF_TO_SHADER_ATTRIBUTE[name];
    }

private:
    std::map<std::string, std::string> GLTF_TO_SHADER_ATTRIBUTE {
        {"POSITION", "position"},
        {"NORMAL", "normal"}
    };
    
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