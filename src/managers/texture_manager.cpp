#include "managers/texture_manager.hpp"


TextureManager::TextureManager(Renderer &renderer): renderer(renderer) {}

void TextureManager::add_texture(std::string texture_name, std::string filename, std::string filepath) {
    GLuint texture_id = load_texture("container.jpg", "./assets/textures/");
    textures.emplace(texture_name, texture_id);
}

GLuint TextureManager::get_texture(std::string texture_name) {
    return textures[texture_name];
}