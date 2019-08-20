#ifndef _GL_BUFFER_HPP
#define _GL_BUFFER_HPP
#include "Engine/renderer/buffer.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"

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
    OpenGLIndexBuffer(void* indices, uint32_t count, uint32_t size);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual uint32_t get_count() const override { return count; }
    virtual int get_type() const override { return enginegl::shader_to_gl_type(type); }

private:
    engine::ShaderDataType type;
    uint32_t index_buffer;
    uint32_t count;
};

} // Namespace
#endif