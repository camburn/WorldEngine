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
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::upload_u_mat4(const std::string& u_name, const glm::mat4& matrix) {
    if (uniforms.count(u_name) == 0)
        return;
    GLuint location = uniforms[u_name].index;
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::upload_u_vec4(const std::string& u_name, const glm::vec4& vec) {
    if (uniforms.count(u_name) == 0)
        return;
    GLuint location = uniforms[u_name].index;
    glUniform4fv(location, 1, glm::value_ptr(vec));
}

void Shader::upload_u_vec3(const std::string& u_name, const glm::vec3& vec) {
    if (uniforms.count(u_name) == 0)
        return;
    GLuint location = uniforms[u_name].index;
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(vec));
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

    const GLsizei bufSize = 32; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);
    //uniforms.reserve(count);

    for (i = 0; i < count; i++)
    {
        glGetActiveUniform(shader_id, i, bufSize, &length, &size, &type, name);
        ENGINE_INFO("Uniform #{0}, Type: {1}, Size: {2} Name: {3}", i, enginegl::GLENUM_NAMES.at(type), size, name);
        uniforms.try_emplace(std::string{name}, i, std::string{name}, type, size);
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
        GLint attribute_location= glGetAttribLocation(shader_id, name.c_str());
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
#if 0
void Shader::program_resources() {
    // TODO: Use this (newer method)
    // This might require a later version of OpenGL
    GLint numActiveAttribs = 0;
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(shader_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
    glGetProgramInterfaceiv(shader_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    std::vector<GLchar> nameData(256);
    std::vector<GLenum> properties;
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    std::vector<GLint> values(properties.size());
    for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        glGetProgramResourceiv(shader_id, GL_PROGRAM_INPUT, attrib, properties.size(),
            &properties[0], values.size(), NULL, &values[0]);

        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(shader_id, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
        std::string name((char*)&nameData[0], nameData.size() - 1);
        ENGINE_INFO("Name: {0]", name);
    }
}
#endif

void Shader::on_ui_render(bool draw) {
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    static std::string last_error = "";

    ImGui::Begin("Shader Editor");
    if (ImGui::Button("Rebuild Shaders")) {
        if (recompile()) {
            last_error = "";
        } else {
            last_error = enginegl::get_last_error();
        }
    }
    ImGui::TextColored({0.8f, 0.1f, 0.1f, 1.0f}, last_error.c_str());
    
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
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16),
            flags,
            nullptr
        );
        ImGui::TreePop();
    }
    ImGui::End();
}

}  // Namespace