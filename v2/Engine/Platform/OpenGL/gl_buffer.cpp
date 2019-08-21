#include "engine.hpp"
#include "gl_buffer.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace enginegl {

OpenGLVertexBuffer::OpenGLVertexBuffer(void *vertices, uint32_t size) {
    buffer_size = size;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    ENGINE_TRACE("Vertex buffer {0} created", vertex_buffer);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    glDeleteBuffers(1, &vertex_buffer);
    ENGINE_TRACE("Vertex buffer {0} garbage collected", vertex_buffer);
}

void OpenGLVertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
}

void OpenGLVertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

struct data_row {
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec2 texcoord;
};

void OpenGLVertexBuffer::read_data() {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    std::vector<data_row> data;
    data.resize(buffer_size/sizeof(data_row));
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, data.data());
    ENGINE_WARN("Reading buffer data");
}

// IndexBuffer

OpenGLIndexBuffer::OpenGLIndexBuffer(void *indices, uint32_t count, uint32_t size): count(count) {
    // Initialiser for unkown data type
    type = engine::ShaderDataType::uShort; // TODO: this is broken, need to resolve type correctly
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    ENGINE_TRACE("Index buffer {0} created", index_buffer);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count): count(count) {
    // Initialiser for floating uint32_t sized data
    type = engine::ShaderDataType::uInt;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    ENGINE_TRACE("Index buffer {0} created", index_buffer);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    glDeleteBuffers(1, &index_buffer);
    ENGINE_TRACE("Index buffer {0} garbage collected", index_buffer);
}

void OpenGLIndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
}

void OpenGLIndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // Namespace