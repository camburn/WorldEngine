#ifndef __RAYCAST_HPP
#define __RAYCAST_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "Engine/renderer/camera.hpp"

namespace engine {

glm::vec3 cast_ray(std::shared_ptr<Camera> &camera);

}
#endif