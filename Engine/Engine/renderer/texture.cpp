#include "engine.hpp"
#include "texture.hpp"

#include "Platform/OpenGL/gl_texture.hpp"

namespace engine {

std::shared_ptr<Texture2D> Texture2D::create(const std::string& path) {
    ENGINE_INFO("Loading Texture2D {0}", path);
    return std::make_shared<enginegl::GLTexture2D>(path);
}
std::shared_ptr<Texture2D> Texture2D::create(uint32_t width, uint32_t height) {
    ENGINE_INFO("Creating Texture2D {0}x{1}",  width, height);
    return std::make_shared<enginegl::GLTexture2D>(width, height);
}

std::shared_ptr<TextureHDR> TextureHDR::create(const std::string& path) {
    ENGINE_INFO("Loading HDR Texture {0}", path);
    return std::make_shared<enginegl::GLTextureHDR>(path);
}

std::shared_ptr<TextureCubeMap> TextureCubeMap::create(
        uint32_t width, uint32_t height, engine::ImageFormat image_format, bool generate_mipmaps,
        engine::Filter min_filter, engine::Filter mag_filter
    ) {
    ENGINE_INFO("Creating Cube Map {0}x{1}", width, height);
    return std::make_shared<enginegl::GLTextureCubeMap>(
        width, height, image_format, generate_mipmaps, min_filter, mag_filter
    );
}

std::shared_ptr<TextureDepth> TextureDepth::create(uint32_t width, uint32_t height) {
    ENGINE_INFO("Creating Depth Map {0}x{1}", width, height);
    return std::make_shared<enginegl::GLTextureDepth>(width, height);
}

} //namespace