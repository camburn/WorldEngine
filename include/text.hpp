#pragma once
#ifndef TEXT_INCLUDED
#define TEXT_INCLUDED

#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#undef HAVE_UNISTD_H
#include FT_FREETYPE_H

#include "graphics/shader.hpp"

void TextInit();
void TextUpdate(int width, int height);
void TextDraw(char *text_data, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

#endif