#ifndef _DEBUG_DRAW_HPP
#define _DEBUG_DRAW_HPP

#include <glm/glm.hpp>
#include "Platform/OpenGL/gl_draw_debug.hpp"

namespace engine {

void draw_line(glm::vec3 a, glm::vec3 b) {
    enginegl::gl_draw_line(a, b);
}

}

#endif