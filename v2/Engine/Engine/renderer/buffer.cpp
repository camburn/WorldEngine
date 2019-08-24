#include "engine.hpp"
#include "buffer.hpp"
#include "Platform/OpenGL/gl_buffer.hpp"

namespace engine {

std::shared_ptr<VertexBuffer> VertexBuffer::create(void* vertices, uint32_t size) {
    return std::make_shared<enginegl::OpenGLVertexBuffer>(vertices, size);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count) {
    return std::make_shared<enginegl::OpenGLIndexBuffer>(indices, count);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(void* indices, uint32_t count, uint32_t size) {
    return std::make_shared<enginegl::OpenGLIndexBuffer>(indices, count, size);
}

} //namespace