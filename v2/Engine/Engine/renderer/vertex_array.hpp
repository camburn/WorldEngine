#ifndef _VERTEX_ARRAY_HPP
#define _VERTEX_ARRAY_HPP
#include <vector>
#include <memory>

#include "Engine/renderer/buffer.hpp"


namespace engine {

static unsigned int array_id = 10000;

class VertexArray {
public:
    VertexArray(): id(array_id++) {}
    virtual ~VertexArray() {}
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual unsigned int get_id() const {return id;}

    virtual void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) = 0;
    virtual void set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer) = 0;

    virtual const std::vector<std::shared_ptr<VertexBuffer>>& get_vertex_buffers() const = 0;
    virtual const std::shared_ptr<IndexBuffer>& get_index_buffer() const = 0;

    static VertexArray* create();
    static VertexArray* create(unsigned int vao);
private:
    const unsigned int id;
};

} // Namespace

#endif