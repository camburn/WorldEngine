#ifndef _VERTEX_ARRAY_HPP
#define _VERTEX_ARRAY_HPP
#include <vector>
#include <memory>

#include "Engine/renderer/buffer.hpp"


namespace engine {

static unsigned int array_id = 10000;

enum class DrawMode {
    TRIANGLES,
    TRIANGLE_STRIP
};

class VertexArray {
public:
    VertexArray(): id(array_id++) {}
    virtual ~VertexArray() {}
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual uint32_t mode() const = 0;

    virtual uint32_t get_id() const = 0;

    virtual void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer, bool generate_attrib_pointers=true) = 0;
    virtual void set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer) = 0;

    virtual void set_array_count(const uint32_t count) = 0;
    virtual uint32_t get_array_count() = 0;

    virtual const std::vector<std::shared_ptr<VertexBuffer>>& get_vertex_buffers() const = 0;
    virtual const std::shared_ptr<IndexBuffer>& get_index_buffer() const = 0;

    static std::shared_ptr<VertexArray> create(DrawMode mode = DrawMode::TRIANGLES);
    static std::shared_ptr<VertexArray> create(unsigned int vao, DrawMode mode = DrawMode::TRIANGLES);
protected:
    DrawMode draw_mode;
private:
    const unsigned int id;
};

} // Namespace

#endif