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

    renderer.active().set_uniform("lightPointPos", State::get_point_light(0).get_position());
    renderer.active().set_uniform("lightPos", light_pos);
    renderer.active().set_uniform("viewPos", view_pos);
    renderer.active().set_uniform("objectColor", glm::vec3(1.0f));
    renderer.active().set_uniform("lightColor", glm::vec3(1.0f));

    renderer.active().set_uniform("pcf_samples", &pcf_samples, U_INT);
    renderer.active().set_uniform("shadow_map_bias", &shadow_map_bias, U_FLOAT);
    renderer.active().set_uniform("cube_map_bias", &cube_map_bias, U_FLOAT);
    renderer.active().set_uniform("use_point_shadow", &use_point_shadow, U_BOOL);
    renderer.active().set_uniform("use_direction_shadow", &use_direction_shadow, U_BOOL);
}

void State::set_light_pos(glm::vec3 pos) {
    light_pos = pos;
}

glm::vec3 &State::get_light_pos() {
    return light_pos;
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

void State::set_direction_light(glm::vec3 pos) {
    direction_light.update_position(pos);
}

void State::create_point_light(glm::vec3 pos) {
    point_lights.emplace_back(PointLight {pos});
}

void State::update_point_light(int index, glm::vec3 pos) {
    point_lights[index].update_position(pos);
}

PointLight State::get_point_light(int index) {
    return point_lights[index];
}

glm::mat4 State::generate_light_matrix() {
    glm::mat4 light_projection = glm::ortho(-12.0f, 12.0f, -12.0f, 12.0f, 1.0f, 25.0f);
    //light_projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 25.0f);
    glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3( 1, 0, 0));
    return light_projection * light_view;
}

std::vector<glm::mat4> PointLight::generate_light_matrix() {
    int width = 1024;
    int height = 1024;
    float aspect = (float)width/(float)height;
    float near = 0.1f;

    glm::mat4 light_projection = glm::perspective(
        glm::radians(90.0f), 
        aspect, 
        near, 
        far_plane
    );

    cube_map.clear();
    cube_map.push_back(
        light_projection * glm::lookAt(
            position, 
            position + glm::vec3( 1.0f,  0.0f,  0.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f)
        )
    );
    cube_map.push_back(
        light_projection * glm::lookAt(
            position, 
            position + glm::vec3(-1.0f,  0.0f,  0.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f)
        )
    );
    cube_map.push_back(
        light_projection * glm::lookAt(
            position, 
            position + glm::vec3( 0.0f,  1.0f,  0.0f), 
            glm::vec3(0.0f,  0.0f,  1.0f)
        )
    );
    cube_map.push_back(
        light_projection * glm::lookAt(
            position, 
            position + glm::vec3( 0.0f, -1.0f,  0.0f), 
            glm::vec3(0.0f,  0.0f, -1.0f)
        )
    );
    cube_map.push_back(
        light_projection * glm::lookAt(
            position, 
            position + glm::vec3( 0.0f,  0.0f,  1.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f)
        )
    );
    cube_map.push_back(
        light_projection * glm::lookAt(
            position, 
            position + glm::vec3( 0.0f,  0.0f, -1.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f)
        )
    );
    return cube_map;
}

std::vector<glm::mat4> PointLight::get_cube_map() {
    return cube_map;
}

void Light::update_position(glm::vec3 new_pos) {
    position = new_pos;
}

void State::light_settings(bool* p_open) {
    const float pos_interval = 1.0f;
    const ImS32 s32_one = 1;
    const float bias_interval = 0.00005f;
    if (!ImGui::Begin("Settings", p_open))
	{
		ImGui::End();
		return;
	}
    if (ImGui::TreeNode("Direction Light Settings")) {
        ImGui::Checkbox("Cast Shadows", &use_direction_shadow);
        ImGui::Checkbox("Animate", &animate_direction_light);
        ImGui::InputScalar("PCF Samples",     ImGuiDataType_S32,    &pcf_samples, true ? &s32_one : NULL, NULL, "%u");
        ImGui::InputScalar("Bias",   ImGuiDataType_Float,  &shadow_map_bias, true ? &bias_interval : NULL);
        float X = light_pos.x;
        float Y = light_pos.y;
        float Z = light_pos.z;
        ImGui::InputScalar("X",   ImGuiDataType_Float,  &X, true ? &pos_interval : NULL);
        ImGui::InputScalar("Y",   ImGuiDataType_Float,  &Y, true ? &pos_interval : NULL);
        ImGui::InputScalar("Z",   ImGuiDataType_Float,  &Z, true ? &pos_interval : NULL);
        if (X != light_pos.x || Y != light_pos.y || Z != light_pos.z){
            set_light_pos(glm::vec3(X, Y, Z));
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Point Light Settings")) {
        ImGui::Checkbox("Cast Shadows", &use_point_shadow);
        ImGui::InputScalar("Bias",   ImGuiDataType_Float,  &cube_map_bias, true ? &bias_interval : NULL);
        glm::vec3 pos = point_lights[0].get_position();
        float X = pos.x;
        float Y = pos.y;
        float Z = pos.z;
        ImGui::InputScalar("X", ImGuiDataType_Float, &X, true ? &pos_interval : NULL);
        ImGui::InputScalar("Y", ImGuiDataType_Float, &Y, true ? &pos_interval : NULL);
        ImGui::InputScalar("Z", ImGuiDataType_Float, &Z, true ? &pos_interval : NULL);
        if (X != pos.x || Y != pos.y || Z != pos.z){
            update_point_light(0, glm::vec3(X, Y, Z));
        }
        ImGui::TreePop();
    }
    ImGui::End();
}
