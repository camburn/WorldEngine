#include "engine.hpp"
#include "buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"

namespace engine {

VertexArray* VertexArray::create() {

    return new enginegl::OpenGLVertexArray();
}

} //namespace