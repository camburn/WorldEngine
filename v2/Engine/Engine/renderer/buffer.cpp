#include "engine.hpp"
#include "buffer.hpp"
#include "Platform/OpenGL/gl_buffer.hpp"

namespace engine {

VertexBuffer* VertexBuffer::create(float* vertices, uint32_t size) {
    return new enginegl::OpenGLVertexBuffer(vertices, size);
}

IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t count) {
    return new enginegl::OpenGLIndexBuffer(indices, count);
}

}