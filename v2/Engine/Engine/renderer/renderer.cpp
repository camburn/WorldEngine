#include "engine.hpp"
#include "renderer.hpp"

#include "imgui.h"

namespace engine {

Renderer::SceneState* Renderer::scene_state = new Renderer::SceneState;


void Renderer::begin_scene(
        const std::shared_ptr<Camera> camera,
        const SceneData data
    ) {
    scene_state->view_projection_matrix = camera->get_view_projection_matrix();
    renderer_api->clear(data.clear_color);
    renderer_api->update_screen(data.width, data.height);
}

void Renderer::end_scene() {

}

void Renderer::submit(
        const std::shared_ptr<Shader>& shader, 
        const std::shared_ptr<engine::VertexArray>& vertex_array,
        const glm::mat4& model) {
    shader->bind();
    shader->upload_u_mat4("u_view_projection", scene_state->view_projection_matrix);
    shader->upload_u_mat4("u_model", model);
    renderer_api->draw_indexed(vertex_array);
}

void Renderer::on_ui_render(bool draw){};

void Renderer::submit_node(
        const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity,
        NodeObject &node_object, glm::mat4 global_transform
    ) {
    glm::mat4 current_transform = global_transform * node_object.get_matrix();
    shader->upload_u_mat4("u_model", current_transform);

    std::vector<int> used_texture_units;

    for (PrimitiveObject prim: node_object.mesh.primitives) {
        // Update uniforms (color, textures)
        shader->upload_u_vec4("u_color", prim.material.color);
        // Submit texture
        for (auto& texture: prim.material.textures) {
            if (texture.texture_id != -1) {
                renderer_api->map_texture_unit(texture.texture_id, texture.texture_unit);
                used_texture_units.push_back(texture.texture_unit);
            }
        }
        // Custom uniforms will override material ones, need to merge the
        // representations
        for (auto const &[name, data]: entity->uniform_vec4_data) {
            shader->upload_u_vec4(name, data);
        }
        // Submit draw call with VAO
        renderer_api->draw_indexed(prim.vao);

        for (int unit: used_texture_units) {
            renderer_api->map_texture_unit(0, unit);
        }
    }

    for (NodeObject child: node_object.children) {
        submit_node(shader, entity, child, current_transform);
    }
}

void Renderer::submit_entity(const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity){
    shader->bind();
    // send global uniforms
    shader->upload_u_mat4("u_view_projection", scene_state->view_projection_matrix);
    
    entity->update_buffers(shader);
    if (std::dynamic_pointer_cast<GltfEntity>(entity)) {
        submit_node(
            shader, entity, std::static_pointer_cast<GltfEntity>(entity)->get_node(),
            entity->uniform_mat4_data["u_model"]
        );

        return;
    }

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