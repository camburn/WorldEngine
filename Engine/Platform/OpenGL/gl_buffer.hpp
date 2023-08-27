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

class OpenGLUniformbuffer : public engine::UniformBuffer {
public:
    OpenGLUniformbuffer(void* data, uint32_t size);

    virtual ~OpenGLUniformbuffer();

    virtual void bind() const override;
    void bind(const uint32_t binding_point) const override;
    void bind_to_shader(const uint32_t shader_id, const std::string binding_name) const override;
    void bind(const uint32_t binding_point, const uint32_t offset, const uint32_t size) const override;
    virtual void unbind() const override;

    void update(void* data) const override;
    void update(void* data, const uint32_t offset, const uint32_t size) const override;
    virtual void read_data() override;

private:
    uint32_t buffer_size;
    uint32_t uniform_buffer;
    engine::BufferLayout m_layout;
};

class OpenGLShaderStorageBuffer : public engine::ShaderStorageBuffer {
public:
    OpenGLShaderStorageBuffer(void* vertices, uint32_t size);

    virtual ~OpenGLShaderStorageBuffer();

    virtual void bind() const override;
    void bind(uint32_t index) const;

    virtual void unbind() const override;

    virtual void read_data() override;

    virtual const engine::BufferLayout& get_layout() const override { return m_layout; }
    virtual void set_layout(const engine::BufferLayout& layout) override { m_layout = layout; }

private:
    uint32_t buffer_size;
    uint32_t ssbo;
    engine::BufferLayout m_layout;
};

class OpenGLIndexBuffer: public engine::IndexBuffer {
public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
    OpenGLIndexBuffer(std::shared_ptr<OpenGLIndexBuffer> &other, uint32_t count, uint32_t offset);
    OpenGLIndexBuffer(void* indices, uint32_t count, uint32_t size);
    OpenGLIndexBuffer(void* indices, uint32_t count, uint32_t size, uint32_t offset);
    OpenGLIndexBuffer(void* indices, uint32_t count, uint32_t size, uint32_t offset, engine::ShaderDataType type);

    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual uint32_t get_count() const override { return count; }
    virtual engine::ShaderDataType get_type() const override { return type; }
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
    OpenGLDepthMap(const std::shared_ptr<GLTextureCubeMap>& texture);

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