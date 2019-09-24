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

OpenGLIndexBuffer::OpenGLIndexBuffer(std::shared_ptr<OpenGLIndexBuffer> &other, uint32_t count, uint32_t offset): count(count), offset(offset) {
    index_buffer = other->index_buffer;
    type = engine::ShaderDataType::uShort;
}

OpenGLIndexBuffer::OpenGLIndexBuffer(void *indices, uint32_t count, uint32_t size, uint32_t offset): count(count), offset(offset) {
    // Initialiser for unkown data type
    type = engine::ShaderDataType::uShort; // TODO: this is broken, need to resolve type correctly
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    ENGINE_TRACE("Index buffer {0} created", index_buffer);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(void *indices, uint32_t count, uint32_t size): count(count), offset(0) {
    // Initialiser for unkown data type
    type = engine::ShaderDataType::uShort; // TODO: this is broken, need to resolve type correctly
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    ENGINE_TRACE("Index buffer {0} created", index_buffer);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count): count(count), offset(0) {
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

// FrameBuffer

OpenGLFrameBuffer::OpenGLFrameBuffer(uint32_t width, uint32_t height) {
    glGenFramebuffers(1, &frame_buffer);
    glGenRenderbuffers(1, &render_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer);

    ENGINE_TRACE("Frame buffer {0} {1}x{2} created", frame_buffer, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
    glDeleteBuffers(1, &frame_buffer);
    glDeleteBuffers(1, &render_buffer);
    ENGINE_TRACE("Frame buffer {0} garbage collected", frame_buffer);
}

void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height) {
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
}

void OpenGLFrameBuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
}

void OpenGLFrameBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

OpenGLDepthMap::OpenGLDepthMap(const std::shared_ptr<GLTextureDepth>& texture) {
    glGenFramebuffers(1, &frame_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->get_id(), 0
    );
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    ENGINE_TRACE("Depth map {0} created", frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLDepthMap::~OpenGLDepthMap() {
    glDeleteBuffers(1, &frame_buffer);
    ENGINE_TRACE("Depth map {0} garbage collected", frame_buffer);
}

void OpenGLDepthMap::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
}

void OpenGLDepthMap::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // Namespace