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

    state.perspective_matrix = projection_matrix;
    state.view_matrix = view_matrix;

    state.light_point_pos = glm::vec4(get_point_light(0).get_position(), 1);
    state.light_point_color = glm::vec4(1.0f);
    state.light_direction_pos = glm::vec4(light_pos, 1);
    state.light_direction_color = glm::vec4(light_color, 1);
    state.view_pos = glm::vec4(view_pos, 1);

    state.shadow_flags.z = pcf_samples;
    state.map_config.x = shadow_map_bias;
    state.map_config.y = cube_map_bias;
    state.debug_flags.x = (unsigned int)DebugGetFlag("render:draw_normals");
    state.debug_flags.y = (unsigned int)DebugGetFlag("render:disable_lighting");
    state.debug_flags.z = (unsigned int)DebugGetFlag("render:draw_texcoords");

    state.shadow_flags.x = (unsigned int)use_point_shadow;
    state.shadow_flags.y = (unsigned int)use_direction_shadow;

    state.direction_light.ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    state.direction_light.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

    PointLightModel point_light_a;
    point_light_a.position = glm::vec4(get_point_light(0).get_position(), 1);
    light_state.point_lights[0] = point_light_a;
    state.state_flags.z = 1; // No of point lights

    state.direction_light.ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);

    renderer.update_uniforms(state, light_state);

    renderer.activate("default");
    // Set world state

    renderer.active().set_uniform("objectColor", glm::vec3(0.0f));
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
    const float light_interval = 0.1f;
    const ImS32 s32_one = 1;
    const ImU32 u32_one = 1;
    const float bias_interval = 0.00005f;
    const float cube_bias_interval = 0.005f;
    if (!ImGui::Begin("Settings", p_open))
    {
        ImGui::End();
        return;
    }
    if (ImGui::TreeNode("Direction Light Settings")) {
        ImGui::Checkbox("Cast Shadows", &use_direction_shadow);
        ImGui::Checkbox("Animate", &animate_direction_light);
        ImGui::InputScalar("PCF Samples", ImGuiDataType_U32, &pcf_samples, true ? &u32_one : NULL, NULL, "%u");
        ImGui::InputScalar("Bias", ImGuiDataType_Float, &shadow_map_bias, true ? &bias_interval : NULL);
        float X = light_pos.x;
        float Y = light_pos.y;
        float Z = light_pos.z;
        ImGui::InputScalar("X", ImGuiDataType_Float, &X, true ? &pos_interval : NULL);
        ImGui::InputScalar("Y", ImGuiDataType_Float, &Y, true ? &pos_interval : NULL);
        ImGui::InputScalar("Z", ImGuiDataType_Float, &Z, true ? &pos_interval : NULL);
        float R = light_color.r;
        float G = light_color.g;
        float B = light_color.b;
        ImGui::InputScalar("R", ImGuiDataType_Float, &R, true ? &light_interval : NULL);
        ImGui::InputScalar("G", ImGuiDataType_Float, &G, true ? &light_interval : NULL);
        ImGui::InputScalar("B", ImGuiDataType_Float, &B, true ? &light_interval : NULL);
        if (X != light_pos.x || Y != light_pos.y || Z != light_pos.z){
            set_light_pos(glm::vec3(X, Y, Z));
        }
        if (R != light_color.r || G != light_color.g || B != light_color.b){
            light_color = glm::vec3(R, G, B);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Point Light Settings")) {
        ImGui::Checkbox("Cast Shadows", &use_point_shadow);
        ImGui::InputScalar("Bias", ImGuiDataType_Float, &cube_map_bias, true ? &cube_bias_interval : NULL);
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
        vec3 point_light_color = point_lights[0].get_color();
        float R = point_light_color.r;
        float G = point_light_color.g;
        float B = point_light_color.b;
        ImGui::InputScalar("R", ImGuiDataType_Float, &R, true ? &light_interval : NULL);
        ImGui::InputScalar("G", ImGuiDataType_Float, &G, true ? &light_interval : NULL);
        ImGui::InputScalar("B", ImGuiDataType_Float, &B, true ? &light_interval : NULL);
        if (R != point_light_color.r || G != point_light_color.g || B != point_light_color.b){
            point_lights[0].update_color(glm::vec3(R, G, B));
        }
        ImGui::TreePop();
    }
    ImGui::End();
}
