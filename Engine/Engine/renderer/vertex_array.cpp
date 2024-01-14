#include "engine.hpp"
#include "vertex_array.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"

namespace engine {

std::shared_ptr<VertexArray> VertexArray::create(DrawMode mode) {
    return std::make_shared<enginegl::OpenGLVertexArray>(mode);
}

std::shared_ptr<VertexArray> VertexArray::create(unsigned int vao, DrawMode mode) {
    return std::make_shared<enginegl::OpenGLVertexArray>(vao, mode);
}

} //namespace