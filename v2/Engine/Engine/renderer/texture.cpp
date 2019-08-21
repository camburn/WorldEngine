#include "engine.hpp"
#include "texture.hpp"

#include "Platform/OpenGL/gl_texture.hpp"

namespace engine {

std::shared_ptr<Texture2D> Texture2D::create(const std::string& path) {
    ENGINE_INFO("Loading Texture {0}", path);
    return std::make_shared<enginegl::GLTexture2D>(path);
}

//std::shared_ptr<Texture2D> Texture2D::create(const void* address) {
//    return std::make_shared<enginegl::GLTexture2D>(address);
//}

} //namespace