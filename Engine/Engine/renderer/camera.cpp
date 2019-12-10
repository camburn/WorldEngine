#include "engine.hpp"

#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"


namespace engine {

std::shared_ptr<Camera> main_camera {nullptr};

void set_main_camera(std::shared_ptr<Camera> camera) {
    main_camera = camera;
}

std::shared_ptr<Camera> get_main_camera() {
    return main_camera;
}

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : Camera(glm::ortho(left, right, bottom, top, -10.0f, 10.0f), glm::mat4(1.0f)) {
    view_projection_matrix = projection_matrix * view_matrix;
}

OrthographicCamera::OrthographicCamera(
    float left, float right, float bottom, float top, float near_plane, float far_plane)
        : Camera(glm::ortho(left, right, bottom, top, near_plane, far_plane), glm::mat4(1.0f)) {
    view_projection_matrix = projection_matrix * view_matrix;
}

void OrthographicCamera::recalculate_view_matrix() {
    // Ortho camera only rotates on Z
    //glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
    //    glm::rotate(glm::mat4(1.0f),
    //    glm::radians(rotation.z), glm::vec3(0, 0, 1));
    glm::mat4 view_matrix = glm::lookAt(position, look_at, up);

    //view_matrix = glm::inverse(transform);
    view_projection_matrix = projection_matrix * view_matrix;
}

void OrthographicCamera::on_ui_render(bool display) {
    
    if (display) {
        ImGui::Begin("Camera Window");
        ImGui::InputFloat3("Position", (float*)&position, 1, 0);
        ImGui::InputFloat3("Rotation", (float*)&rotation, 1, 0);
        ImGui::End();
    }
}

void OrthographicCamera::set_view(glm::vec3 new_position, glm::vec3 new_look_at, glm::vec3 new_up) {
    position = new_position;
    look_at = new_look_at;
    up = new_up;
    recalculate_view_matrix();
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near_plane, float far_plane)
        : Camera(glm::perspective(glm::radians(fov), aspect, near_plane, far_plane), 
          glm::lookAt(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))) {
    view_projection_matrix = projection_matrix * view_matrix;
}

void PerspectiveCamera::set_proj_matrix(float fov, float aspect, float near_plane, float far_plane) {
    projection_matrix = glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
    view_projection_matrix = projection_matrix * view_matrix;
}

void PerspectiveCamera::set_view(glm::vec3 new_position, glm::vec3 new_look_at, glm::vec3 new_up) {
    position = new_position;
    look_at = new_look_at;
    up = new_up;
    recalculate_view_matrix();
}

void PerspectiveCamera::recalculate_view_matrix() {
    view_matrix = glm::lookAt(position, look_at, up);
    /*
    glm::mat4 transform = 
        glm::rotate(view_matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
        glm::rotate(view_matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
        glm::rotate(view_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    view_matrix = transform;
    */

    view_projection_matrix = projection_matrix * view_matrix;
}

void PerspectiveCamera::on_ui_render(bool display) {
    
    if (display) {
        ImGui::Begin("Camera Window");
        ImGui::InputFloat3("Position", (float*)&position, 1, 0);
        ImGui::InputFloat3("Rotation", (float*)&rotation, 1, 0);
        ImGui::InputFloat3("Look at", (float*)&look_at, 1, 0);
        ImGui::InputFloat3("Up", (float*)&up, 1, 0);
        ImGui::End();
    }
}


} //Namespace