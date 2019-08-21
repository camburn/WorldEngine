#include "engine.hpp"

#include "gl_vertex_array.hpp"
#include "glad/glad.h"

#include "Platform/OpenGL/gl_names.hpp"


namespace enginegl {

static std::map<engine::ShaderDataType, GLenum> shader_to_gl_types {
    {engine::ShaderDataType::Float,  GL_FLOAT},
    {engine::ShaderDataType::Float2, GL_FLOAT},
    {engine::ShaderDataType::Float3, GL_FLOAT},
    {engine::ShaderDataType::Float4, GL_FLOAT},
    {engine::ShaderDataType::Int,    GL_INT},
    {engine::ShaderDataType::Int2,   GL_INT},
    {engine::ShaderDataType::Int3,   GL_INT},
    {engine::ShaderDataType::Int4,   GL_INT},
    {engine::ShaderDataType::Mat3,   GL_FLOAT},
    {engine::ShaderDataType::Mat4,   GL_FLOAT},
    {engine::ShaderDataType::Bool,   GL_BOOL},
    {engine::ShaderDataType::uChar,  GL_UNSIGNED_BYTE},
    {engine::ShaderDataType::Char,   GL_BYTE},
    {engine::ShaderDataType::Short,  GL_SHORT},
    {engine::ShaderDataType::uShort, GL_UNSIGNED_SHORT},
    {engine::ShaderDataType::uInt,   GL_UNSIGNED_INT},
    {engine::ShaderDataType::uChar,  GL_UNSIGNED_BYTE}
};

GLenum shader_to_gl_type(const engine::ShaderDataType type) {
    return shader_to_gl_types[type];
}

OpenGLVertexArray::OpenGLVertexArray (GLuint vao) {
    vao_id = vao;
}

OpenGLVertexArray::OpenGLVertexArray () {
    glGenVertexArrays(1, &vao_id);
    // Too new
    // glCreateVertexArrays(1, &vao_id);
    ENGINE_TRACE("Vertex array {0} created", vao_id);
}

OpenGLVertexArray::~OpenGLVertexArray () {
    ENGINE_TRACE("Vertex array {0} garbage collected", vao_id);
    glDeleteVertexArrays(1, &vao_id);
}

void OpenGLVertexArray::bind() const {
    glBindVertexArray(vao_id);
}

void OpenGLVertexArray::unbind() const {
    glBindVertexArray(0);
}

void OpenGLVertexArray::describe_layout(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer) const {
    uint32_t index = 0;
    const auto& layout = vertex_buffer->get_layout();
    for (const auto& element: layout) {
        ENGINE_INFO("Describing index {0} as {1}", index, element.name);
        ENGINE_INFO("glVertexAttribPointer({0}, {1}, {2}, {3}, {4}, {5})",
            index,
            element.get_component_count(),
            GLENUM_NAMES.at(shader_to_gl_type(element.type)),
            element.normalised ? "GL_TRUE": "GL_FALSE",
            layout.get_stride(),
            element.offset
        );
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
            index,
            element.get_component_count(),
            shader_to_gl_type(element.type),
            element.normalised ? GL_TRUE : GL_FALSE,
            layout.get_stride(),
            (const void*)element.offset
        );
        index++;
    }
}

void OpenGLVertexArray::describe_layouts() const {
    
    for (auto& vertex_buffer: vertex_buffers) {
        describe_layout(vertex_buffer);
    }
}

void OpenGLVertexArray::disable_layout(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer) const {
    uint32_t index = 0;
    const auto& layout = vertex_buffer->get_layout();
    for (const auto& element: layout) {
        glDisableVertexAttribArray(index);
        index++;
    }
}

void OpenGLVertexArray::disable_layouts() const {
    
    for (auto& vertex_buffer: vertex_buffers) {
        disable_layout(vertex_buffer);
    }
}

void OpenGLVertexArray::add_vertex_buffer(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer, bool generate_attrib_pointers) {
    if (generate_attrib_pointers) {
        glBindVertexArray(vao_id);
        describe_layout(vertex_buffer);
        glBindVertexArray(0);
    }
    vertex_buffers.push_back(vertex_buffer);
}

void OpenGLVertexArray::set_index_buffer(const std::shared_ptr<engine::IndexBuffer>& buffer) {
    glBindVertexArray(vao_id);
    index_buffer = buffer;
    index_buffer->bind();
}

} // namespace