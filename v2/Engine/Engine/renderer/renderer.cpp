#include "engine.hpp"
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
}

void Renderer::submit_entity(const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity){
    shader->bind();
    // send global uniforms
    shader->upload_u_mat4("u_view_projection", scene_data->view_projection_matrix);
    // Send Entity uniforms
    for (auto const &[name, data]: entity->uniform_vec4_data) {
        shader->upload_u_vec4(name, data);
    }
    for (auto const &[name, data]: entity->uniform_mat4_data) {
        shader->upload_u_mat4(name, data);
    }
    // When do we update buffers?
    entity->update_buffers(shader);
    for (auto const &tex_id: entity->texture_ids) {
        renderer_api->map_texture(tex_id);
    }
    // TODO: Draw triangles if no indices
    for (auto const &vao: *entity) {
        renderer_api->draw_indexed(vao);
    }
    // TODO: Smart stuff to build an appropriate VAO for the bound shader if not available
}

} //namespace