#include "engine.hpp"

#include "glad/glad.h"

#include "gl_renderer_api.hpp"


namespace glengine {

void GLRendererAPI::clear(const glm::vec4 &color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRendererAPI::draw_indexed(const std::shared_ptr<engine::VertexArray>& vertex_array) {
    vertex_array->bind();
    glDrawElements(GL_TRIANGLES, vertex_array->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
}

} //namespace