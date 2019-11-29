#include "engine.hpp"
#include "collision.hpp"

namespace engine {

AABBCollider::AABBCollider(): position(glm::vec3{0.0f}), size(glm::vec3{1.0f}) {
    calculate_minimals();
}

AABBCollider::AABBCollider(glm::vec3 position, glm::vec3 size): position(position), size(size){
    calculate_minimals();
}

void AABBCollider::calculate_minimals() {
    glm::vec3 half_size = glm::vec3(size.x / 2, size.y / 2, size.z / 2);
    min_corner = position - half_size;
    max_corner = position + half_size;
}

bool AABBCollider::intersect(Ray ray) {
    float t1 = (min_corner.x - ray.origin.x) * ray.unit_direction.x;
    float t2 = (max_corner.x - ray.origin.x) * ray.unit_direction.x;
    float t3 = (min_corner.y - ray.origin.y) * ray.unit_direction.y;
    float t4 = (max_corner.y - ray.origin.y) * ray.unit_direction.y;
    float t5 = (min_corner.z - ray.origin.z) * ray.unit_direction.z;
    float t6 = (max_corner.z - ray.origin.z) * ray.unit_direction.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    float ray_length_at_intersection;
    // if tmax < 0, ray is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0) {
        ray_length_at_intersection = tmax;
        return false;
    }

    // if tmin > tmax ray doesn't intersect AABB
    if (tmin > tmax) {
        ray_length_at_intersection = tmax;
        return false;
    }

    ray_length_at_intersection = tmin;
    return true;
}

std::vector<glm::vec4> AABBCollider::points() {
    
    std::vector<glm::vec4> data;

    auto p1 = glm::vec4(min_corner, 1.0f);
    auto p2 = glm::vec4((min_corner + glm::vec3{size.x, 0.0f, 0.0f}), 1.0f);
    auto p3 = glm::vec4((min_corner + glm::vec3{0.0f, 0.0f, size.z}), 1.0f);
    auto p4 = glm::vec4((min_corner + glm::vec3{size.x, 0.0f, size.z}), 1.0f);

    auto p5 = glm::vec4((min_corner + glm::vec3{0.0f, size.y, 0.0f}), 1.0f);
    auto p6 = glm::vec4((min_corner + glm::vec3{size.x, size.y, 0.0f}), 1.0f);
    auto p7 = glm::vec4((min_corner + glm::vec3{0.0f, size.y, size.z}), 1.0f);
    auto p8 = glm::vec4((min_corner + glm::vec3{size.x, size.y, size.z}), 1.0f);

    data.push_back(p1);
    data.push_back(p2);
    data.push_back(p3);
    data.push_back(p4);
    data.push_back(p5);
    data.push_back(p6);
    data.push_back(p7);
    data.push_back(p8);

    return data;
}

BoxCollider::BoxCollider(): size(glm::vec3{1.0f}) {
    calculate_minimals();
    transform.disable_scale();
    transform.set_translation(glm::vec3{0.0f});
}

BoxCollider::BoxCollider(glm::vec3 position, glm::vec3 size): size(size){
    transform.disable_scale();
    transform.set_translation(position);
    calculate_minimals();
}

void BoxCollider::calculate_minimals() {
    glm::vec3 half_size = glm::vec3(size.x / 2, size.y / 2, size.z / 2);
    min_corner = glm::vec3{0.0f} - half_size;
    max_corner = glm::vec3{0.0f} + half_size;
}

bool BoxCollider::intersect(Ray ray) {
    float t_min = 0.0f;
    float t_max = 100000.0f;

    glm::mat4 m_model = transform.get_model_matrix();
    glm::vec3 obb_position_worldspace {m_model[3].x, m_model[3].y, m_model[3].z};

    glm::vec3 delta = obb_position_worldspace - ray.origin;

    { // X-axis test
        glm::vec3 xaxis {m_model[0].x, m_model[0].y, m_model[0].z};
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray.direction, xaxis);

        if (fabs(f) > 0.001f) {
            float t1 = (e + min_corner.x) / f;
            float t2 = (e + max_corner.x) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }
            if (t2 < t_max) { t_max = t2; }
            if (t1 > t_min) { t_min = t1; }
            if (t_max < t_min ) { return false; }
        } else {  // Parallel case
            if (-e + min_corner.x > 0.0f || -e + max_corner.x < 0.0f) {
                return false;
            }
        }
    }
    { // Y-axis test
        glm::vec3 yaxis {m_model[1].x, m_model[1].y, m_model[1].z};
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray.direction, yaxis);

        if (fabs(f) > 0.001f) {
            float t1 = (e + min_corner.y) / f;
            float t2 = (e + max_corner.y) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }
            if (t2 < t_max) { t_max = t2; }
            if (t1 > t_min) { t_min = t1; }
            if (t_max < t_min ) { return false; }
        } else {  // Parallel case
            if (-e + min_corner.y > 0.0f || -e + max_corner.y < 0.0f) {
                return false;
            }
        }
    }
    { // Z-axis test
        glm::vec3 zaxis {m_model[2].x, m_model[2].y, m_model[2].z};
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray.direction, zaxis);

        if (fabs(f) > 0.001f) {
            float t1 = (e + min_corner.z) / f;
            float t2 = (e + max_corner.z) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }
            if (t2 < t_max) { t_max = t2; }
            if (t1 > t_min) { t_min = t1; }
            if (t_max < t_min ) { return false; }
        } else {  // Parallel case
            if (-e + min_corner.z > 0.0f || -e + max_corner.z < 0.0f) {
                return false;
            }
        }
    }
    float intersection_distance = t_min;
    return true;
}

std::vector<glm::vec4> BoxCollider::points() {
    
    std::vector<glm::vec4> data;
    glm::mat4 m_model = transform.get_model_matrix();

    auto p1 = glm::vec4(min_corner, 1.0f);
    auto p2 = glm::vec4((min_corner + glm::vec3{size.x, 0.0f, 0.0f}), 1.0f);
    auto p3 = glm::vec4((min_corner + glm::vec3{0.0f, 0.0f, size.z}), 1.0f);
    auto p4 = glm::vec4((min_corner + glm::vec3{size.x, 0.0f, size.z}), 1.0f);

    auto p5 = glm::vec4((min_corner + glm::vec3{0.0f, size.y, 0.0f}), 1.0f);
    auto p6 = glm::vec4((min_corner + glm::vec3{size.x, size.y, 0.0f}), 1.0f);
    auto p7 = glm::vec4((min_corner + glm::vec3{0.0f, size.y, size.z}), 1.0f);
    auto p8 = glm::vec4((min_corner + glm::vec3{size.x, size.y, size.z}), 1.0f);

    data.push_back(m_model * p1);
    data.push_back(m_model * p2);
    data.push_back(m_model * p3);
    data.push_back(m_model * p4);
    data.push_back(m_model * p5);
    data.push_back(m_model * p6);
    data.push_back(m_model * p7);
    data.push_back(m_model * p8);

    return data;
}

}

namespace engine_debug {

void draw_box_collider(engine::Collider& box) {
    auto p = box.points();
    engine_debug::draw_simple_line(p[0], p[4], box.debug_color);
    engine_debug::draw_simple_line(p[1], p[5], box.debug_color);
    engine_debug::draw_simple_line(p[2], p[6], box.debug_color);
    engine_debug::draw_simple_line(p[3], p[7], box.debug_color);

    engine_debug::draw_simple_line(p[0], p[1], box.debug_color);
    engine_debug::draw_simple_line(p[2], p[3], box.debug_color);
    engine_debug::draw_simple_line(p[4], p[5], box.debug_color);
    engine_debug::draw_simple_line(p[6], p[7], box.debug_color);

    engine_debug::draw_simple_line(p[0], p[2], box.debug_color);
    engine_debug::draw_simple_line(p[1], p[3], box.debug_color);
    engine_debug::draw_simple_line(p[4], p[6], box.debug_color);
    engine_debug::draw_simple_line(p[5], p[7], box.debug_color);
}


}


