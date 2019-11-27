#include "engine.hpp"
#include "raycast.hpp"

namespace engine {

//RayHit::RayHit(): hit_target(nullptr) {

//}

RayHit::RayHit(Collider &collider, Ray ray, glm::vec3 hit_point):
        hit_target(collider), ray(ray), hit_point(hit_point) {
    hit = true;
}

Ray cast_ray(std::shared_ptr<Camera> &camera){
    int width = Application::get().get_window().get_width();
    int height = Application::get().get_window().get_height();
    auto mouse_pos = ImGui::GetMousePos();
    float mouse_x = mouse_pos.x;
    float mouse_y = mouse_pos.y;

    if (mouse_x <= -FLT_MAX || mouse_y <= -FLT_MAX) {
        ENGINE_WARN("Bad raycast detected");
        return Ray(false);
    }

    float x = (2.0f * mouse_x) / width - 1.0f;
    float y = 1.0f - (2.0f * mouse_y) / height;
    float z = 1.0f;

    // Normalised Device Coordinates
    glm::vec3 ray_nds = glm::vec3(x, y, z);
    // 4d Homogeneous clip coordinates
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
    // 4d eye(camera) coordinates
    glm::vec4 ray_eye = glm::inverse(camera->get_projection_matrix()) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
    // 4d world coordinates
    glm::vec3 ray_world = glm::vec3(inverse(camera->get_view_matrix()) * ray_eye);
    ray_world = glm::normalize(ray_world);

    return Ray(camera->get_position(), ray_world);
}

}