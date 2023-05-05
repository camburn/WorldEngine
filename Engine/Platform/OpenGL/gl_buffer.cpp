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
    //ENGINE_TRACE("Vertex buffer {0} created", vertex_buffer);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    glDeleteBuffers(1, &vertex_buffer);
    //ENGINE_TRACE("Vertex buffer {0} garbage collected", vertex_buffer);
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

//SSBO
OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(void* data, uint32_t size) {
    buffer_size = size;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_READ);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    ENGINE_TRACE("SSBO {0} created", ssbo);
}

OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer() {
    glDeleteBuffers(1, &ssbo);
    //ENGINE_TRACE("Vertex buffer {0} garbage collected", vertex_buffer);
}

void OpenGLShaderStorageBuffer::bind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
}

void OpenGLShaderStorageBuffer::bind(uint32_t index) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
}

void OpenGLShaderStorageBuffer::unbind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void OpenGLShaderStorageBuffer::read_data() {
    
}

// IndexBuffer

OpenGLIndexBuffer::OpenGLIndexBuffer(std::shared_ptr<OpenGLIndexBuffer> &other, uint32_t count, uint32_t offset): count(count), offset(offset) {
    index_buffer = other->index_buffer;
    type = other->get_type();
}

OpenGLIndexBuffer::OpenGLIndexBuffer(
        void *indices, uint32_t count, uint32_t size, uint32_t offset, engine::ShaderDataType type
        ) : count(count), offset(offset), type(type)
    {
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    ENGINE_TRACE("Index buffer {0} created", index_buffer);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(
        void *indices, uint32_t count, uint32_t size, uint32_t offset
        ) : OpenGLIndexBuffer(
            indices, count, size, offset, engine::ShaderDataType::uShort
        )
    { }


OpenGLIndexBuffer::OpenGLIndexBuffer(
        void *indices, uint32_t count, uint32_t size
        ): OpenGLIndexBuffer(
            indices, count, size, 0, engine::ShaderDataType::uShort
        )
    { }

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count
        ): OpenGLIndexBuffer(
            indices, count, count * sizeof(uint32_t), 0, engine::ShaderDataType::uInt
        )
    { }

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    glDeleteBuffers(1, &index_buffer);
    //ENGINE_TRACE("Index buffer {0} garbage collected", index_buffer);
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
    //glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    ENGINE_TRACE("Frame buffer {0} {1}x{2} resized", frame_buffer, width, height);
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

    ENGINE_TRACE("Depth map buffer {0} created", frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLDepthMap::OpenGLDepthMap(const std::shared_ptr<GLTextureCubeMap>& texture) {
    glGenFramebuffers(1, &frame_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glFramebufferTexture(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->get_id(), 0
    );
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    ENGINE_TRACE("Depth map buffer {0} created", frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLDepthMap::~OpenGLDepthMap() {
    glDeleteBuffers(1, &frame_buffer);
    ENGINE_TRACE("Depth map buffer {0} garbage collected", frame_buffer);
}

void OpenGLDepthMap::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
}

void OpenGLDepthMap::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // Namespace