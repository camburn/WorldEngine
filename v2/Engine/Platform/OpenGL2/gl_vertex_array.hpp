#ifndef _GL_VERTEX_ARRAY_HPP
#define _GL_VERTEX_ARRAY_HPP

#include <vector>
#include <memory>

#include "glad/glad.h"

#include "Engine/renderer/vertex_array.hpp"

namespace enginegl {

class OpenGLVertexArray: public engine::VertexArray {
public:
    OpenGLVertexArray ();
    OpenGLVertexArray (GLuint vao);
    ~OpenGLVertexArray ();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void add_vertex_buffer(const std::shared_ptr<engine::VertexBuffer>& vertex_buffer, bool generate_attrib_pointers=true) override;
    virtual void set_index_buffer(const std::shared_ptr<engine::IndexBuffer>& index_buffer) override;

    virtual const std::vector<std::shared_ptr<engine::VertexBuffer>>& get_vertex_buffers() const {return vertex_buffers; }
    virtual const std::shared_ptr<engine::IndexBuffer>& get_index_buffer() const { return index_buffer; }

private:
    uint32_t vao_id;
    std::vector<std::shared_ptr<engine::VertexBuffer>> vertex_buffers;
    std::shared_ptr<engine::IndexBuffer> index_buffer;
};

} // Namespace

#endif