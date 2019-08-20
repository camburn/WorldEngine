#include <sstream>
#include "engine.hpp"
#include "gl_texture.hpp"

#include "imgui.h"

namespace enginegl {

struct TexData {
    GLuint id;
    int width;
    int height;
    std::string name;
};

std::vector<TexData> textures;

GLuint buffer_image(tinygltf::Sampler &sampler, tinygltf::Image &image) {
    // Do not load loaded textures
    for (auto& data: textures) {
        if (image.uri == data.name) {
            return data.id;
        }
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, sampler.wrapR);

    GLenum format;
    switch (image.component)
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        ENGINE_ERROR("Unsupported texture component count");
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        image.width, image.height,
        0, format, image.pixel_type, &image.image.at(0)
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    textures.push_back(TexData{texture_id, image.height, image.width, image.uri});
    return texture_id;
}

void display_loaded_textures(bool display) {
    ImGui::Begin("Textures");
    for (TexData data: textures) {
        std::ostringstream iss;
        iss << data.name << "(" << data.width << "x" << data.height << ")";
        std::string name = iss.str();
        if (ImGui::TreeNode(name.c_str())) {
            ImGui::Image((void*)(intptr_t)data.id, ImVec2(data.width, data.height));
            ImGui::TreePop();
        }
    }
    
    ImGui::End();
}

}