#ifndef __RAYCAST_HPP
#define __RAYCAST_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "Engine/renderer/camera.hpp"

namespace engine {

class Collider;

class Ray {
public:
    Ray(): origin(glm::vec3{0.0f}), direction(glm::vec3{0.0f}), unit_direction(1.0f / direction) {}
    Ray(bool valid): valid(valid) {}
    Ray(glm::vec3 origin, glm::vec3 direction): 
        origin(origin), direction(direction), unit_direction(1.0f / direction) {}

    glm::vec3 project(float distance) {
        return origin + (direction * distance);
    }

    bool valid = true;
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 unit_direction;
};

class RayHit {
public:
    RayHit(Collider &collider, Ray ray, glm::vec3 hit_point);

    bool hit;

    glm::vec3 hit_point;
    Ray ray;
    Collider &hit_target;
};

Ray cast_ray(std::shared_ptr<Camera> &camera);

}
#endif