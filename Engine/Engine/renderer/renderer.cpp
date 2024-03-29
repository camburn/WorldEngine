#include "engine.hpp"
#include "renderer.hpp"

#include "imgui.h"

namespace engine {

Renderer::SceneState* Renderer::scene_state = new Renderer::SceneState;

std::vector <std::function<void(bool)>> call_later;

void Renderer::begin_scene(
    const std::shared_ptr<Camera> camera,
    const SceneData data,
    const int render_flags
) {
    if ((render_flags & CULL_FACE_FRONT) == CULL_FACE_FRONT) {
        renderer_api->cull_face_front(true);
        std::function<void(bool)> tidy_cull_face = std::bind(&RendererAPI::cull_face_front, renderer_api, false);
        call_later.push_back(tidy_cull_face);
        //need to tidy this state on end_scene
    }
}


void Renderer::begin_scene(
        const std::shared_ptr<Camera> camera,
        const SceneData data
    ) {
    scene_state->view_projection_matrix = camera->get_view_projection_matrix();
    renderer_api->clear(data.clear_color);
    renderer_api->update_screen(data.width, data.height);
}

void Renderer::end_scene() {
    for (auto &func : call_later) {
        func(false);
    }
    call_later.clear();
}

void Renderer::submit(
        const std::shared_ptr<Shader>& shader, 
        const std::shared_ptr<engine::VertexArray>& vertex_array,
        const glm::mat4& model) {
    shader->bind();
    shader->upload_u_mat4("u_view_projection", scene_state->view_projection_matrix);
    shader->upload_u_mat4("u_model", model);
    if (vertex_array->get_index_buffer()) {
        renderer_api->draw_indexed(vertex_array);
    } else {
        renderer_api->draw_triangles(vertex_array);
    }
    
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
            if (texture.texture_id != -1 && shader->uniform_supported(texture.u_name))  {
                int texture_unit = shader->uniform_texture_unit(texture.u_name);
                renderer_api->map_texture_unit(texture.texture_id, texture_unit);
                used_texture_units.push_back(texture_unit);
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

void Renderer::submit_entity(
        const std::shared_ptr<Shader>& shader,
        std::shared_ptr<Entity> &entity,
        Transform &transform
    ){
    shader->bind();
    // send global uniforms
    shader->upload_u_mat4("u_view_projection", scene_state->view_projection_matrix);

    glm::mat4 u_model = transform.get_model_matrix();
    entity->update_buffers(shader);
    if (std::dynamic_pointer_cast<GltfEntity>(entity)) {
        submit_node(
            shader, entity, std::static_pointer_cast<GltfEntity>(entity)->get_node(),
            u_model
        );
        return;
    }

    // Send Entity uniforms
    for (auto const &[name, data]: entity->uniform_vec4_data) {
        shader->upload_u_vec4(name, data);
    }
    for (auto const &[name, data]: entity->uniform_mat4_data) {
        // TODO: Currently if a seperate model matrix is defined, it will
        // override the object matrix - should be added
        if (name == "u_model") {
            u_model *= data;
            shader->upload_u_mat4(name, u_model);
            continue;
        }
        shader->upload_u_mat4(name, data);
        
    }
    std::vector<uint32_t> units;
    for (auto const &[name, texture]: std::static_pointer_cast<CustomEntity>(entity)->textures) {
        if (shader->uniform_supported(name)) {
            uint32_t unit = shader->uniform_texture_unit(name);
            texture->bind(unit);
            units.push_back(unit);
        }
    }
    // When do we update buffers?
    entity->update_buffers(shader);
    for (auto const &tex_id: entity->texture_ids) {
        renderer_api->map_texture(tex_id);
    }
    if (std::static_pointer_cast<CustomEntity>(entity)->elements_set()) {
        for (auto const &vao: *entity) {
            renderer_api->draw_indexed(vao);
        }
    }
    else {
        for (auto const &vao: *entity) {
            renderer_api->draw_triangles(vao);
        }
    }
    for (auto unit: units) {
        renderer_api->map_texture_unit(0, unit);
    }
    // TODO: Draw triangles if no indices

    //auto const &vao = std::dynamic_pointer_cast<CustomEntity>(entity)->get_shader_vaos(shader->get_id());
    //if (vao) {
    //    renderer_api->draw_indexed(vao);
    //}
    // TODO: Smart stuff to build an appropriate VAO for the bound shader if not available

    //shader->bind();
}

} //namespace