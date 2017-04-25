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
void DebugCreateLine(float x1, float y1, float z1, float x2, float y2, float z2);
void DebugCreateLine(float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b);
void DebugCreateLine(vec3 point1, vec3 point2, vec3 color);
void DebugDraw(mat4 Projection, mat4 View);