#include "engine.hpp"
#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/shader_loader.hpp"
#include "Platform/OpenGL/gl_names.hpp"

namespace engine {
// TODO: Create OpenGL shader class

// What are the commanalities between differnt graphics shader implmentations


Shader::Shader(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path)
    : vs_shader_path(vertex_shader_file_path), fs_shader_path(fragment_shader_file_path) {
    GLuint id = build();
    ENGINE_ASSERT(id, "Shaders could not compile");
    shader_id = id;
    inspect_uniforms();
    inspect_attributes();
}

Shader::~Shader() {
    glDeleteProgram(shader_id);
}

void Shader::recompile() {
    ENGINE_INFO("Recompiling shaders");
    GLuint id = build();
    if (!id) return;
    glDeleteProgram(shader_id);
    shader_id = id;
    inspect_uniforms();
    inspect_attributes();
}

GLuint Shader::build() {
    return enginegl::build_program(vs_shader_path, fs_shader_path);
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
    GLuint location = uniforms[u_name].index;
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::inspect_uniforms() {
    GLint i;
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 32; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    for (i = 0; i < count; i++)
    {
        glGetActiveUniform(shader_id, i, bufSize, &length, &size, &type, name);
        ENGINE_INFO("Uniform #{0}, Type: {1}, Size: {2} Name: {3}", i, enginegl::GLENUM_NAMES.at(type), size, name);
        uniforms.try_emplace(std::string{name}, i, std::string{name}, type, size);
    }
}

void Shader::inspect_attributes() {
    GLint numActiveAttribs = 0;
    glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);

    GLint maxAttribNameLength = 0;
    glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
    std::vector<GLchar> nameData(maxAttribNameLength);
    for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
        glGetActiveAttrib(shader_id, attrib, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
        std::string name((char*)&nameData[0], actualLength);
        ENGINE_INFO("Attribute #{0} Type: {1} Name: {2}", attrib, enginegl::GLENUM_NAMES.at(type), name);
    }
}

void Shader::program_resources() {
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

}  // Namespace