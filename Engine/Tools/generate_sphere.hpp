#ifndef __GENERATE_SPHERE_HPP
#define __GENERATE_SPHERE_HPP

#include <vector>

#include "glm/glm.hpp"

#include "Engine/entity.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"

namespace engine {

std::shared_ptr<CustomEntity> generate_sphere();

}
#endif