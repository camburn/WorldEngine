#ifndef _GL_VERTEX_ARRAY_HPP
#define _GL_VERTEX_ARRAY_HPP

#include <vector>
#include <memory>

#include <glad/glad.h>

#include "Engine/renderer/vertex_array.hpp"

namespace enginegl {

class OpenGLVertexArray: public engine::VertexArray {
public:
    OpenGLVertexArray ();
    OpenGLVertexArray (GLuint vao);
    ~OpenGLVertexArray ();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual uint32_t get_id() const override { return vao_id; }

    virtual void add_vertex_buffer(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer, bool generate_attrib_pointers=true) override;
    virtual void set_index_buffer(const std::shared_ptr<engine::IndexBuffer>& index_buffer) override;
    virtual void set_array_count(const uint32_t count) override { element_count = count; }
    virtual uint32_t get_array_count() override { return element_count; }

    virtual const std::vector<std::shared_ptr<engine::VertexBuffer>>& get_vertex_buffers() const override {return vertex_buffers; }
    virtual const std::shared_ptr<engine::IndexBuffer>& get_index_buffer() const override { return index_buffer; }

private:
    uint32_t vao_id;
    uint32_t element_count;
    std::vector<std::shared_ptr<engine::VertexBuffer>> vertex_buffers;
    std::shared_ptr<engine::IndexBuffer> index_buffer;

    void describe_layouts() const;
    void describe_layout(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer) const;

    void disable_layouts() const;
    void disable_layout(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer) const;
};

GLenum shader_to_gl_type(const engine::ShaderDataType type);

} // Namespace

#endif