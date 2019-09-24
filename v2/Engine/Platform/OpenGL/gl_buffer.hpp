#ifndef _GL_BUFFER_HPP
#define _GL_BUFFER_HPP
#include "Engine/renderer/buffer.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"
#include "Platform/OpenGL/gl_texture.hpp"

namespace enginegl {

class OpenGLVertexBuffer: public engine::VertexBuffer {
public:
    OpenGLVertexBuffer(void *vertices, uint32_t size);

    virtual ~OpenGLVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void read_data() override;

    virtual const engine::BufferLayout& get_layout() const override { return m_layout; }
    virtual void set_layout(const engine::BufferLayout& layout) override { m_layout = layout; }

private:
    uint32_t buffer_size;
    uint32_t vertex_buffer;
    engine::BufferLayout m_layout;
};

class OpenGLIndexBuffer: public engine::IndexBuffer {
public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
    OpenGLIndexBuffer(std::shared_ptr<OpenGLIndexBuffer> &other, uint32_t count, uint32_t offset);
    OpenGLIndexBuffer(void* indices, uint32_t count, uint32_t size);
    OpenGLIndexBuffer(void* indices, uint32_t count, uint32_t size, uint32_t offset);

    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual uint32_t get_count() const override { return count; }
    virtual int get_type() const override { return enginegl::shader_to_gl_type(type); }
    virtual uint32_t get_offset() const override { return offset; }

private:
    engine::ShaderDataType type;
    uint32_t index_buffer;
    uint32_t count;
    uint32_t offset;
};

class OpenGLFrameBuffer: public engine::FrameBuffer {
public:
    OpenGLFrameBuffer(uint32_t width, uint32_t height);

    virtual ~OpenGLFrameBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;
    virtual void resize(uint32_t width, uint32_t height) override;

private:
    uint32_t buffer_size;
    uint32_t frame_buffer;
    uint32_t render_buffer;
};

class OpenGLDepthMap: public engine::FrameBuffer {
public:
    OpenGLDepthMap(const std::shared_ptr<GLTextureDepth>& texture);

    virtual ~OpenGLDepthMap();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void resize(uint32_t width, uint32_t height) override {};

private:
    uint32_t buffer_size;
    uint32_t frame_buffer;
};

} // Namespace
#endif