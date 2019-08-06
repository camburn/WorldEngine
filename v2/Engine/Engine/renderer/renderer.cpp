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

    #ifdef OPENGL_COMPATIBILITY
    shader->unbind();
    #endif
}

void Renderer::submit_entity(const std::shared_ptr<Shader>& shader, Entity &entity){
    shader->bind();
    // send global uniforms
    shader->upload_u_mat4("u_view_projection", scene_data->view_projection_matrix);
    // Send Entity uniforms
    for (auto const &[name, data]: entity.uniform_vec4_data) {
        shader->upload_u_vec4(name, data);
    }
    for (auto const &[name, data]: entity.uniform_mat4_data) {
        shader->upload_u_mat4(name, data);
    }
    entity.update_buffers(shader);
    // TODO: Draw triangles if no indices
    renderer_api->draw_indexed(entity.get_vao());
    // TODO: Smart stuff to build an appropriate VAO for the bound shader if not available
    #ifdef OPENGL_COMPATIBILITY
    shader->unbind();
    #endif
}

} //namespace