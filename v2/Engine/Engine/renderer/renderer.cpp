#include "engine.hpp"
#include "renderer.hpp"

#include "imgui.h"

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

void Renderer::on_ui_render(bool draw){};

void Renderer::submit_node(
        const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity,
        NodeObject &node_object, glm::mat4 global_transform
    ) {
    // Update node uniforms (transform)
    glm::mat4 current_transform = node_object.transform_matrix * global_transform;
    shader->upload_u_mat4("u_model", current_transform);

    for (PrimitiveObject prim: node_object.mesh.primitives) {
        // Update uniforms (color, textures)
        shader->upload_u_vec4("u_color", prim.material.color);
        // Submit texture
        if (prim.material.texture_set)
            renderer_api->map_texture(prim.material.texture_id);
        // Submit draw call with VAO
        renderer_api->draw_indexed(prim.vao);
    }

    for (NodeObject child: node_object.children) {
        submit_node(shader, entity, child, current_transform);
    }
}

void Renderer::submit_entity(const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity){
    shader->bind();
    // send global uniforms
    shader->upload_u_mat4("u_view_projection", scene_data->view_projection_matrix);
    

    entity->update_buffers(shader);

    submit_node(
        shader, entity, std::static_pointer_cast<GltfEntity>(entity)->get_node(),
        entity->uniform_mat4_data["u_model"]
    );

    return;


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

    //shader->bind();

    
}

} //namespace