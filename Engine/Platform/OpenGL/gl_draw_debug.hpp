#ifndef _GL_DEBUG_DRAW_HPP
#define _GL_DEBUG_DRAW_HPP

#include <glm/glm.hpp>
#include "glad/glad.h"
#include "Platform/OpenGL/gl_buffer.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"


namespace enginegl {


void gl_draw_line(glm::vec3 a, glm::vec3 b);

/*
Draw a line from a cube

a Get a 1x1x1 cube
Get a forward vector i.e. v3(0, 1, 0)

Determine line direction vector (normalize(b - a))
then rotate cube to face direction

Then determine distance (glm::distance(a, b))
Then scale forward vector by distance
*/
void new_frame();
void draw_line(glm::vec4 a, glm::vec4 b, glm::vec4 color);
void draw_buffers();

}

#endif