#include "engine.hpp"

#include "glm/glm.hpp"

#include "renderer.hpp"

namespace engine {

Renderer::SceneData* Renderer::scene_data = new Renderer::SceneData;


 void Renderer::begin_scene(const std::shared_ptr<Camera> camera, const glm::vec4& clear_color) {
    scene_data->view_projection_matrix = camera->get_view_projection_matrix();
    renderer_api->clear(clear_color);
}

void Renderer::end_scene() {

}

void Renderer::submit(
        const std::shared_ptr<Shader>& shader, 
        const std::shared_ptr<engine::VertexArray>& vertex_array,
        const glm::mat4& model) {
    shader->bind();
    shader->upload_u_mat4("u_view_projection", scene_data->view_projection_matrix);
    shader->upload_u_mat4("u_model", model);
    renderer_api->draw_indexed(vertex_array);

    #ifdef OPENGL_COMPATIBILITY
    shader->unbind();
    #endif
}

} //namespace