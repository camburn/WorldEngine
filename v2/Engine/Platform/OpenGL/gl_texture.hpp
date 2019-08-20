#ifndef _GL_TEXTURE_HPP
#define _GL_TEXTURE_HPP
#include <glad/glad.h>
#include "tiny_gltf.h"

namespace enginegl {
GLuint buffer_image(tinygltf::Sampler &sampler, tinygltf::Image &image);
void display_loaded_textures(bool display);
}

#endif