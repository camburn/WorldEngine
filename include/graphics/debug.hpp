#pragma once

#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include "graphics/shader.hpp"

void DebugInit();
void DebugCreateLine(vec3 point1, vec3 point2);
void DebugDraw(mat4 Projection, mat4 View);