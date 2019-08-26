#ifndef _GL_TEXTURE_HPP
#define _GL_TEXTURE_HPP

#include <memory>
#include <string>
#include <sstream>
#include <glad/glad.h>
#include "tiny_gltf.h"

#include "Engine/renderer/texture.hpp"

namespace enginegl {

class GLTexture2D: public engine::Texture2D {
public:
    GLTexture2D(const std::string path);
    GLTexture2D(const void* address);

    virtual ~GLTexture2D();

    virtual uint32_t get_width() const { return width; }
    virtual uint32_t get_height() const  { return height; }

    virtual void bind(uint32_t slot = 0) const;

private:
    GLuint texture_id;
    uint32_t width;
    uint32_t height;
    std::string path;
};


GLuint buffer_image(tinygltf::Sampler &sampler, tinygltf::Image &image);
void display_loaded_textures(bool display);
}

#endif