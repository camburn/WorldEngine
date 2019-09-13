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

    GLenum format;
    GLenum storage_format;
    switch (channels)
    {
    case 1:
        format = GL_RED;
        storage_format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        storage_format = GL_RGB8;
        break;
    case 4:
        format = GL_RGBA;
        storage_format = GL_RGBA8;
        break;
    default:
        ENGINE_ERROR("Unsupported texture component count");
        return;
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, storage_format,
        width, height, 0, format, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    /*
    // Note Modern Opengl 4+ way to create textures
    glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
    
    glTextureStorage2D(texture_id, 1, GL_RGB8, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureSubImage2D(texture_id, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    */

    textures.push_back(TexData{texture_id, (int)height, (int)width, path});
    glBindTexture(GL_TEXTURE_2D, 0);
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

GLTextureHDR::GLTextureHDR(const std::string path) { 
    int img_width, img_height, channels;
    //stbi_set_flip_vertically_on_load(true);
    ENGINE_ASSERT(stbi_is_hdr(path.c_str()), "File is not a HDR file");
    float* data = stbi_loadf(path.c_str(), &img_width, &img_height, &channels, 0);
    ENGINE_ASSERT(data, "Failed to load image from path");
    width = img_width;
    height = img_height;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Is this the devil that kills OpenGL 2.1 support? GL_RGB16F
    
    #ifdef OPENGL_COMPATIBILITY
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, width, height, 0, GL_RGB, GL_FLOAT, data);
    #else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
    #endif
    
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    textures.push_back(TexData{texture_id, (int)height, (int)width, path});
}

GLTextureHDR::~GLTextureHDR() {
    glDeleteTextures(1, &texture_id);
}

void GLTextureHDR::bind(uint32_t slot) const {
    #ifdef OPENGL_COMPATIBILITY
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    #else
    glBindTextureUnit(slot, texture_id);
    #endif
}

GLTextureDepth::GLTextureDepth(uint32_t width, uint32_t height) {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
    textures.push_back(TexData{texture_id, (int)height, (int)width, "Depth Texture"});
}

GLTextureDepth::~GLTextureDepth() {
    glDeleteTextures(1, &texture_id);
}

void GLTextureDepth::bind(uint32_t slot) const {
    #ifdef OPENGL_COMPATIBILITY
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    #else
    glBindTextureUnit(slot, texture_id);
    #endif
}

GLTextureCubeMap::GLTextureCubeMap(uint32_t width, uint32_t height) {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16,
            width, height, 0, GL_RGB, GL_FLOAT, nullptr
        );
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    textures.push_back(TexData{texture_id, (int)height, (int)width, "Cubemap"});
}

void GLTextureCubeMap::bind(uint32_t slot) const {
    #ifdef OPENGL_COMPATIBILITY
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    #else
    glBindTextureUnit(slot, texture_id);
    #endif
}

GLTextureCubeMap::~GLTextureCubeMap() {
    glDeleteTextures(1, &texture_id);
}

void GLTextureCubeMap::set_data(uint32_t face_index) {
    ENGINE_ASSERT(face_index < 6, "Cube map only has 6 faces, index out of range");
    // Load data into the cube map from the framebuffer
    // This will have to be expanded to handle load from depth buffers aswell
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_index, texture_id, 0
    );
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
        image.width, image.height,
        0, format, image.pixel_type, &image.image.at(0)
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    textures.push_back(TexData{texture_id, image.height, image.width, image.uri});
    glBindTexture(GL_TEXTURE_2D, 0);
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