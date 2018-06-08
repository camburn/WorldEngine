#include <string>
#include <vector>
#include <unordered_map>
#include "graphics/renderer.hpp"


#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

class TextureManager {
public: 
    TextureManager(Renderer &renderer);
    void add_texture(
        std::string texture_name, 
        std::string filename, 
        std::string filepath
    );
    GLuint get_texture(std::string texture_name);
private:
    Renderer renderer;
    std::unordered_map<std::string, GLuint> textures;
};

#endif
