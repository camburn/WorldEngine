#ifndef _GL_TEXTURE_HPP
#define _GL_TEXTURE_HPP

#include <memory>
#include <string>
#include <sstream>
#include <glad/glad.h>
#include "tiny_gltf.h"

#include "Engine/renderer/texture.hpp"

namespace enginegl {

class GLTexture {
public:
    GLuint get_id() { return texture_id; }

protected:
    GLuint texture_id;
};

class GLTexture2D: public engine::Texture2D, public GLTexture {
public:
    GLTexture2D(const std::string path);
    GLTexture2D(const void* address);
    GLTexture2D(uint32_t width, uint32_t height);

    virtual ~GLTexture2D();

    virtual uint32_t get_width() const override { return width; }
    virtual uint32_t get_height() const override { return height; }

    virtual void set_data() override;

    virtual void bind(uint32_t slot = 0) const override;
    virtual void unbind() override;

private:
    uint32_t width;
    uint32_t height;
    std::string path;
};

class GLTextureHDR: public engine::TextureHDR, public GLTexture {
public:
    GLTextureHDR(const std::string path);

    virtual ~GLTextureHDR();

    virtual uint32_t get_width() const override { return width; }
    virtual uint32_t get_height() const override { return height; }

    virtual void bind(uint32_t slot = 0) const override;
    virtual void unbind() override;
private:
    uint32_t width;
    uint32_t height;
    std::string path;
};

class GLTextureCubeMap: public engine::TextureCubeMap, public GLTexture {
public:
    GLTextureCubeMap(
        uint32_t width, uint32_t height,
        bool generate_mipmaps,
        engine::Filter min_filter,
        engine::Filter mag_filter
    );

    virtual void set_data(uint32_t face_index, uint32_t mip = 0) override;
    virtual void generate_mipmaps() override;

    virtual ~GLTextureCubeMap();

    virtual uint32_t get_width() const override { return width; }
    virtual uint32_t get_height() const override { return height; }

    virtual void bind(uint32_t slot = 0) const override;
    virtual void unbind() override;
private:
    uint32_t width;
    uint32_t height;
};

class GLTextureDepth: public engine::TextureDepth, public GLTexture {
public:
    GLTextureDepth(uint32_t width, uint32_t height);

    virtual ~GLTextureDepth();

    virtual uint32_t get_width() const override { return width; }
    virtual uint32_t get_height() const override { return height; }

    virtual void bind(uint32_t slot = 0) const override;
    virtual void unbind() override;
private:
    uint32_t width;
    uint32_t height;
};

GLuint buffer_image(tinygltf::Sampler &sampler, tinygltf::Image &image);
void display_loaded_textures(bool display);
}

#endif