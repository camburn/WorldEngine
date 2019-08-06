#include "engine.hpp"
#include "gl_buffer.hpp"
#include "glad/glad.h"

namespace enginegl {

OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size) {
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

// IndexBuffer

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count): count(count) {
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