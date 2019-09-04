#include "engine.hpp"
#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_stdlib.h"

#include "Platform/OpenGL/shader_loader.hpp"
#include "Platform/OpenGL/gl_names.hpp"

namespace engine {
// TODO: Create OpenGL shader class
// What are the commanalities between different graphics shader implmentations

Shader::Shader(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path)
        : vs_shader_path(vertex_shader_file_path), fs_shader_path(fragment_shader_file_path) {

    GLuint id = enginegl::load_build_program(vertex_shader_file_path, fragment_shader_file_path,
        vs_data, fs_data);
    ENGINE_ASSERT(id, "Shaders could not compile");
    shader_id = id;
    //bind();
    inspect_uniforms();
    inspect_attributes();
}

Shader::~Shader() {
    glDeleteProgram(shader_id);
}

bool Shader::recompile() {
    ENGINE_INFO("Recompiling shaders");
    GLuint id = enginegl::build_program(vs_data.data(), fs_data.data());
    if (!id) return false;
    glDeleteProgram(shader_id);
    shader_id = id;
    inspect_uniforms();
    inspect_attributes();
    registered_vertex_arrays.clear();
    return true;
}

GLuint Shader::build() {
    return enginegl::build_program(vs_shader_path, fs_shader_path);
}

void Shader::bind() {
    glUseProgram(shader_id);

    for (auto& [name, uniform]: uniforms) {
        if (uniform.type == GL_SAMPLER_2D) {
            upload_u_int1(uniform.name, uniform.texture_unit);
        }
    }
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::upload_u_mat4(const std::string& u_name, const glm::mat4& matrix) {
    if (!uniform_supported(u_name))
        return;
    GLuint location = uniform_location(u_name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::upload_u_vec4(const std::string& u_name, const glm::vec4& vec) {
    if (!uniform_supported(u_name))
        return;
    GLuint location = uniform_location(u_name);
    glUniform4fv(location, 1, glm::value_ptr(vec));
}

void Shader::upload_u_vec3(const std::string& u_name, const glm::vec3& vec) {
    if (!uniform_supported(u_name))
        return;
    GLuint location = uniform_location(u_name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

void Shader::upload_u_vec3(const std::string& u_name, const std::vector<glm::vec3>& vecs) {
    if (!uniform_supported(u_name))
        return;
    GLuint location = uniform_location(u_name);
    glUniform3fv(location, vecs.size(), glm::value_ptr(vecs[0]));
}

void Shader::upload_u_int1(const std::string& u_name, const GLint& value) {
    if (!uniform_supported(u_name))
        return;
    GLuint location = uniform_location(u_name);
    glUniform1i(location, value);
}

bool Shader::attribute_supported(std::string name) {
    name = GLTF_TO_SHADER_ATTRIBUTE[name];
    for (auto & [index, attribute]: attributes) {
        if (attribute.name == name) {
            return true;
        }
    }
    return false;
}

int Shader::attribute_location(std::string name) {
    name = GLTF_TO_SHADER_ATTRIBUTE[name];
    for (auto & [index, attribute]: attributes) {
        if (attribute.name == name) {
            return attribute.index;
        }
    }
    return -1;
}

bool Shader::uniform_supported(std::string name) {
    if (uniforms.count(name) > 0) {
        return true;
    }
    return false;
}

int Shader::uniform_location(std::string name) {
    if (uniforms.count(name) > 0) {
        return uniforms.at(name).index;
    }
    return -1;
}

int Shader::uniform_texture_unit(std::string name) {
    if (uniforms.count(name) > 0) {
        return uniforms.at(name).texture_unit;
    }
    return -1;
}

std::string Shader::attribute_name_normalise(std::string name) {
    return GLTF_TO_SHADER_ATTRIBUTE[name];
}

void Shader::register_vertex_array(std::shared_ptr<VertexArray> vao) {
    registered_vertex_arrays.emplace(vao->get_id());
}

bool Shader::is_vertex_array_registered(std::shared_ptr<VertexArray> vao) {
    return registered_vertex_arrays.count(vao->get_id()) == 1;
}

void Shader::inspect_uniforms() {
    uniforms.clear();
    GLint i;
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)
    GLint texture_unit = 0;

    const GLsizei bufSize = 32; // maximum name length
    GLchar c_name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);
    //uniforms.reserve(count);

    for (i = 0; i < count; i++) {
        glGetActiveUniform(shader_id, i, bufSize, &length, &size, &type, c_name);
        std::string name = std::string(c_name);
        if (type == GL_SAMPLER_2D) {
            // Assign a texture unit
            uniforms.try_emplace(name, i, name, type, size, texture_unit);
            ENGINE_INFO("Attribute #{0} Type: {1} Name: {2} TextureUnit: {3}", i, enginegl::GLENUM_NAMES.at(type), name, texture_unit);
            texture_unit++;
        } else {
            if (name.find("[") < name.length()) {
                GLint attribute_location=  -2;
                int counter = 0;
                while (true) {
                    name.pop_back(); name.pop_back();
                    name = name + std::to_string(counter) + "]";
                    attribute_location = glGetUniformLocation(shader_id, name.c_str());
                    if (attribute_location == -1)
                        break;
                    ENGINE_INFO("Uniform #{0}, Type: {1}, Size: {2} Name: {3}", attribute_location, enginegl::GLENUM_NAMES.at(type), size, name);
                    uniforms.try_emplace(name, attribute_location, name, type, size);
                    counter++;
                }
            }else {
                GLint attribute_location = glGetUniformLocation(shader_id, name.c_str());
                ENGINE_INFO("Uniform #{0}, Type: {1}, Size: {2} Name: {3}", attribute_location, enginegl::GLENUM_NAMES.at(type), size, name);
                uniforms.try_emplace(name, attribute_location, name, type, size);
            }
        }
    }
}

void Shader::inspect_attributes() {
    // TODO: Consider how we can build an appropriate buffer based on the currently loaded shader
    attributes.clear();  // Does clear deallocate reserved memory?
    GLint numActiveAttribs = 0;
    GLint size = 0;
    GLenum type = 0;
    GLsizei length = 0;
    glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);

    GLint maxAttribNameLength = 0;
    glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
    std::vector<GLchar> nameData(maxAttribNameLength);
    for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        glGetActiveAttrib(shader_id, attrib, nameData.size(), &length, &size, &type, &nameData[0]);
        std::string name = nameData.data();
        GLint attribute_location = glGetAttribLocation(shader_id, name.c_str());
        attributes.try_emplace(attribute_location, attribute_location, name, type, size);
        ENGINE_INFO("Attribute #{0} Type: {1} Name: {2}", attribute_location, enginegl::GLENUM_NAMES.at(type), name);
    }
}

BufferLayout Shader::attribute_layout() {
    std::vector<BufferElement> elements;
    for (auto &[index, attribute]: attributes) {
        elements.emplace_back(
            enginegl::GLENUM_TO_SHADER_TYPES.at(attribute.type), attribute.name);
    }
    return BufferLayout {elements};
}

void Shader::on_ui_render(bool draw) {
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    static std::string last_error = "";

    ImGui::Begin("Shader Editor");

    if (ImGui::TreeNode("Attributes")) {
        for (auto& [index, attribute]: attributes) {
            ImGui::Text("%i - %s", attribute.index, attribute.name.c_str());
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Uniforms")) {
        for (auto& [name, uniform]: uniforms) {
            ImGui::Text("%i - %s", uniform.index, uniform.name.c_str());
        }
        ImGui::TreePop();
    }
    ImGui::Separator();

    if (ImGui::Button("Rebuild Shaders")) {
        if (recompile()) {
            last_error = "";
        } else {
            last_error = enginegl::get_last_error();
        }
    }
    ImGui::TextColored({0.8f, 0.1f, 0.1f, 1.0f}, "%s", last_error.c_str());
    
    if (ImGui::TreeNode("Vertex Shader")) {
        ImGui::InputTextMultiline(
            "Vertex shader",
            &vs_data,
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16),
            flags,
            nullptr
        );
        
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Fragment Shader")) {
        ImGui::InputTextMultiline(
            "Fragment Shader",
            &fs_data,
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 64),
            flags,
            nullptr
        );
        ImGui::TreePop();
    }
    ImGui::End();
}

}  // Namespace