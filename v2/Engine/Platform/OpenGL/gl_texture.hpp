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

    virtual uint32_t get_width() const override { return width; }
    virtual uint32_t get_height() const override { return height; }

    virtual void bind(uint32_t slot = 0) const override;

private:
    GLuint texture_id;
    uint32_t width;
    uint32_t height;
    std::string path;
};

class GLTextureHDR: public engine::TextureHDR {
public:
    GLTextureHDR(const std::string path);

    virtual ~GLTextureHDR();

    virtual uint32_t get_width() const override { return width; }
    virtual uint32_t get_height() const override { return height; }

    virtual void bind(uint32_t slot = 0) const override;

private:
    GLuint texture_id;
    uint32_t width;
    uint32_t height;
    std::string path;
};

class GLTextureCubeMap: public engine::TextureCubeMap {
public:
    GLTextureCubeMap(uint32_t width, uint32_t height);

    virtual void set_data(uint32_t face_index) override;

    virtual ~GLTextureCubeMap();

    virtual uint32_t get_width() const override { return width; }
    virtual uint32_t get_height() const override { return height; }

    virtual void bind(uint32_t slot = 0) const override;

private:
    GLuint texture_id;
    uint32_t width;
    uint32_t height;
};

GLuint buffer_image(tinygltf::Sampler &sampler, tinygltf::Image &image);
void display_loaded_textures(bool display);
}

#endif