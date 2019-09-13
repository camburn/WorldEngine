#include "engine.hpp"
#include "buffer.hpp"
#include "Platform/OpenGL/gl_buffer.hpp"
#include "Platform/OpenGL/gl_texture.hpp"

namespace engine {

std::shared_ptr<VertexBuffer> VertexBuffer::create(void* vertices, uint32_t size) {
    return std::make_shared<enginegl::OpenGLVertexBuffer>(vertices, size);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(std::shared_ptr<IndexBuffer> buffer, uint32_t count, uint32_t offset) {
    auto gl_buffer = std::dynamic_pointer_cast<enginegl::OpenGLIndexBuffer>(buffer);
    return std::make_shared<enginegl::OpenGLIndexBuffer>(gl_buffer, count, offset);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count) {
    return std::make_shared<enginegl::OpenGLIndexBuffer>(indices, count);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(void* indices, uint32_t count, uint32_t size) {
    return std::make_shared<enginegl::OpenGLIndexBuffer>(indices, count, size);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(void* indices, uint32_t count, uint32_t size, uint32_t offset) {
    return std::make_shared<enginegl::OpenGLIndexBuffer>(indices, count, size, offset);
}

std::shared_ptr<FrameBuffer> FrameBuffer::create(uint32_t width, uint32_t height) {
    return std::make_shared<enginegl::OpenGLFrameBuffer>(width, height);
}
std::shared_ptr<FrameBuffer> FrameBuffer::create(
    const std::shared_ptr<TextureDepth>& texture) {

    return std::make_shared<enginegl::OpenGLDepthMap>(
        std::static_pointer_cast<enginegl::GLTextureDepth>(texture)
    );
}

} //namespace