#include "engine.hpp"

#include "gl_vertex_array.hpp"
#include "glad/glad.h"


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
    ENGINE_TRACE("Vertex array {0} created", vao_id);
}

OpenGLVertexArray::~OpenGLVertexArray () {
    ENGINE_TRACE("Vertex array {0} garbage collected", vao_id);
}

void OpenGLVertexArray::bind() const {
    if (index_buffer == nullptr) return;
    for (auto& buffer: vertex_buffers) {
        buffer->bind();
    }
    index_buffer->bind();
}

void OpenGLVertexArray::unbind() const {
    for (auto& buffer: vertex_buffers) {
        buffer->unbind();
    }
    index_buffer->unbind();
}

void OpenGLVertexArray::add_vertex_buffer(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer, bool generate_attrib_pointers) {
    if (generate_attrib_pointers) {
        vertex_buffer->bind();

        uint32_t index = 0;
        const auto& layout = vertex_buffer->get_layout();
        for (const auto& element: layout) {
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
    vertex_buffers.push_back(vertex_buffer);
}

void OpenGLVertexArray::set_index_buffer(const std::shared_ptr<engine::IndexBuffer>& buffer) {
    index_buffer = buffer;
    index_buffer->bind();
}

} // namespace