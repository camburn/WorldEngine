#include "engine.hpp"
#include "vertex_array.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"

namespace engine {

std::shared_ptr<VertexArray> VertexArray::create() {
    return std::make_shared<enginegl::OpenGLVertexArray>();
}

std::shared_ptr<VertexArray> VertexArray::create(unsigned int vao) {
    return std::make_shared<enginegl::OpenGLVertexArray>(vao);
}

} //namespace