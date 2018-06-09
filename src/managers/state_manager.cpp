#include "managers/state_manager.hpp"


State::State(Renderer &state_renderer) 
        : renderer(state_renderer) {
    program_id = renderer.get_shader_id("default");
}

State::State(Renderer &state_renderer, glm::mat4 &proj_mat, glm::mat4 &view_mat)
        : renderer(state_renderer), projection_matrix(proj_mat), view_matrix(view_mat) {
    program_id = renderer.get_shader_id("default");
}

void State::update_state() {
    renderer.activate("default");
    renderer.active().set_uniform(
        "debug_draw_normals", 
        DebugGetFlag("render:draw_normals")
    );
    renderer.active().set_uniform(
        "debug_draw_texcoords", 
        DebugGetFlag("render:draw_normals")
    );
    renderer.active().set_uniform(
        "debug_disable_lighting", 
        DebugGetFlag("render:draw_normals")
    );

    renderer.active().set_uniform("lightPos", light_pos);
    renderer.active().set_uniform("viewPos", view_pos);
    renderer.active().set_uniform("objectColor", glm::vec3(1.0f));
    renderer.active().set_uniform("lightColor", glm::vec3(1.0f));
}

void State::set_light_pos(glm::vec3 pos) {
    light_pos = pos;
}

void State::set_view_pos(glm::vec3 pos) {
    view_pos = pos;
}

GLuint State::get_program() {
    return program_id;
}

void State::set_projection(glm::mat4 matrix) {
    invalid_model_view = true;
    projection_matrix = matrix;
}

void State::set_view(glm::mat4 matrix) {
    invalid_model_view = true;
    view_matrix = matrix;
}

glm::mat4 State::generate_model_view() {
    if (invalid_model_view) {
        cached_model_view = projection_matrix * view_matrix;
        invalid_model_view = false;
    }
    return cached_model_view;
}

glm::mat4 State::generate_light_matrix() {
    glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
    glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3( 1, 0, 0));
    return light_projection * light_view;
}
