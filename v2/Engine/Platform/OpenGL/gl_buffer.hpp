#ifndef _GL_BUFFER_HPP
#define _GL_BUFFER_HPP
#include "Engine/renderer/buffer.hpp"

namespace enginegl {

class OpenGLVertexBuffer: public engine::VertexBuffer {
public:
    OpenGLVertexBuffer(float *vertices, uint32_t size);

    virtual ~OpenGLVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual const engine::BufferLayout& get_layout() const override { return m_layout; }
    virtual void set_layout(const engine::BufferLayout& layout) override { m_layout = layout; }

private:
    uint32_t vertex_buffer;
    engine::BufferLayout m_layout;
};

class OpenGLIndexBuffer: public engine::IndexBuffer {
public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual uint32_t get_count() const { return count; }

private:
    uint32_t index_buffer;
    uint32_t count;
};

} // Namespace
#endif