#include "engine.hpp"
#include "buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"

namespace engine {

VertexArray* VertexArray::create() {

    return new enginegl::OpenGLVertexArray();
}

VertexArray* VertexArray::create(unsigned int vao) {

    return new enginegl::OpenGLVertexArray(vao);
}

} //namespace