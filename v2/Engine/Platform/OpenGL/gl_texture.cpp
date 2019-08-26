#include "engine.hpp"
#include "gl_texture.hpp"

#include "imgui.h"
#include "stb_image.h"

namespace enginegl {

struct TexData {
    GLuint id;
    int width;
    int height;
    std::string name;
};

std::vector<TexData> textures;


GLTexture2D::GLTexture2D(const std::string path) {
    int img_width, img_height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(path.c_str(), &img_width, &img_height, &channels, 0);
    ENGINE_ASSERT(data, "Failed to load image from path");
    width = img_width;
    height = img_height;

    #ifdef OPENGL_COMPATIBILITY
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    #else
    // Note Modern Opengl 4+ way to create textures
    glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
    
    glTextureStorage2D(texture_id, 1, GL_RGB8, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureSubImage2D(texture_id, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    #endif

    textures.push_back(TexData{texture_id, (int)height, (int)width, path});

    stbi_image_free(data);
}

GLTexture2D::GLTexture2D(const void* address) {

}

GLTexture2D::~GLTexture2D() {
    glDeleteTextures(1, &texture_id);
}

void GLTexture2D::bind(uint32_t slot) const {
    #ifdef OPENGL_COMPATIBILITY
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    #else
    glBindTextureUnit(slot, texture_id);
    #endif
}

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
        std::stringstream iss;
        iss << data.name << "(" << data.width << "x" << data.height << ")";
        std::string name = iss.str();
        if (ImGui::TreeNode(name.c_str())) {
            auto size = ImGui::GetWindowSize();
            ImGui::Image((void*)(intptr_t)data.id, ImVec2(size.x - 50, size.x - 50));
            ImGui::TreePop();
        }
    }
    
    ImGui::End();
}

}