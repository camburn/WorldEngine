#include "engine.hpp"

#include "glad/glad.h"

#include "gl_renderer_api.hpp"
#include "gl_vertex_array.hpp"


namespace glengine {

void GLRendererAPI::clear(const glm::vec4 &color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRendererAPI::update_screen(int width, int height) {
    glViewport(0, 0, width, height);
}

void GLRendererAPI::cull_face_front(bool enable) {
    if (enable) {
        glCullFace(GL_FRONT);
    }
    glCullFace(GL_BACK);
}

void GLRendererAPI::map_texture(const unsigned int tex_id) {
    glBindTexture(GL_TEXTURE_2D, tex_id);
}

void GLRendererAPI::map_texture_unit(const unsigned int tex_id, const unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, tex_id);
}

void GLRendererAPI::draw_indexed(const std::shared_ptr<engine::VertexArray>& vertex_array) {
    ENGINE_ASSERT(vertex_array->get_index_buffer(), "Index buffer not set on Vertex Array");
    vertex_array->bind();
    GLenum gl_type = enginegl::shader_to_gl_type(vertex_array->get_index_buffer()->get_type());
    glDrawElements(
        vertex_array->mode(),
        vertex_array->get_index_buffer()->get_count(),
        gl_type,
        reinterpret_cast<const GLvoid *>(vertex_array->get_index_buffer()->get_offset())
    );
    vertex_array->unbind();
}

void GLRendererAPI::draw_triangles(const std::shared_ptr<engine::VertexArray>& vertex_array) {
    vertex_array->bind();
    if (vertex_array->get_drawmode() == engine::DrawMode::TRIANGLES) {
        glDrawArrays(GL_TRIANGLES, 0, vertex_array->get_array_count() );
    }
    else if (vertex_array->get_drawmode() == engine::DrawMode::TRIANGLE_STRIP) {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_array->get_array_count() );
    }
    else {
        ENGINE_WARN("Draw mode not supported");
    }
    vertex_array->unbind();
}

} //namespace