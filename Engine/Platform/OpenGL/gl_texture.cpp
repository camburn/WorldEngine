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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    stbi_set_flip_vertically_on_load(0);
}

GLTexture2D::GLTexture2D(uint32_t width, uint32_t height) {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #ifdef OPENGL_COMPATIBILITY
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, width, height, 0, GL_RGB, GL_FLOAT, 0);
    #else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, 0);
    #endif

    textures.push_back(TexData{texture_id, (int)height, (int)width, "CustomTexture"});
}

void GLTexture2D::set_data() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, texture_id, 0
    );
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

void GLTexture2D::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLTextureHDR::GLTextureHDR(const std::string path) { 
    int img_width, img_height, channels;
    stbi_set_flip_vertically_on_load(1);
    ENGINE_ASSERT(stbi_is_hdr(path.c_str()), "File is not a HDR file");
    float* data = stbi_loadf(path.c_str(), &img_width, &img_height, &channels, 0);
    ENGINE_ASSERT(data, "Failed to load image from path");
    width = img_width;
    height = img_height;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    stbi_set_flip_vertically_on_load(0);
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

void GLTextureHDR::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
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

void GLTextureDepth::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLTextureCubeMap::GLTextureCubeMap(uint32_t width, uint32_t height, engine::ImageFormat image_format,
        bool generate_mipmaps, engine::Filter min_filter, engine::Filter mag_filter
    ) {

    image_format_state = image_format;

    GLenum gl_min_filter = GL_LINEAR;
    if (min_filter == engine::NEAREST) {
        gl_min_filter = GL_NEAREST;
    } else if (min_filter == engine::LINEAR_MIPMAP_LINEAR){
        gl_min_filter = GL_LINEAR_MIPMAP_LINEAR;
    }
    GLenum gl_mag_filter = GL_LINEAR;
    if (mag_filter == engine::NEAREST) {
        gl_mag_filter = GL_NEAREST;
    } else if (mag_filter == engine::LINEAR_MIPMAP_LINEAR){
        ENGINE_ERROR("GL_TEXTURE_MAG_FILTER does not support mip-mapping specify an alternative filter");
    }
    GLint gl_image_format = GL_RGB;
    GLint gl_image_format_internal = GL_RGB16;
    switch(image_format) {
        case engine::RGB:
            gl_image_format = GL_RGB;
            gl_image_format_internal = GL_RGB16;
            break;
        case engine::RGBA:
            gl_image_format = GL_RGBA;
            gl_image_format_internal = GL_RGBA;
            break;
        case engine::DEPTH_COMPONENT:
            gl_image_format = GL_DEPTH_COMPONENT;
            gl_image_format_internal = GL_DEPTH_COMPONENT;
            break;
        default:
            ENGINE_ERROR("Unsupported image format used to store texture data");
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, gl_min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_image_format_internal,
            width, height, 0, gl_image_format, GL_FLOAT, (void*)nullptr
        );
    }
    if (generate_mipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        ENGINE_ERROR("Framebuffer is not complete, this is a problem");
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    textures.push_back(TexData{texture_id, (int)height, (int)width, "Cubemap"});
}

void GLTextureCubeMap::generate_mipmaps() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void GLTextureCubeMap::bind(uint32_t slot) const {
    #ifdef OPENGL_COMPATIBILITY
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    #else
    glBindTextureUnit(slot, texture_id);
    #endif
}

void GLTextureCubeMap::unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLTextureCubeMap::~GLTextureCubeMap() {
    glDeleteTextures(1, &texture_id);
}

void GLTextureCubeMap::set_data(uint32_t face_index, uint32_t mip) {
    ENGINE_ASSERT(face_index < 6, "Cube map only has 6 faces, index out of range");

    GLint attachment = GL_COLOR_ATTACHMENT0;
    switch (image_format_state) {
    case engine::RGB:
        attachment = GL_COLOR_ATTACHMENT0;
        break;
    case engine::RGBA:
        attachment = GL_COLOR_ATTACHMENT0;
        break;
    case engine::DEPTH_COMPONENT:
        attachment = GL_DEPTH_ATTACHMENT;
        break;
    }

    // Load data into the cube map from the framebuffer
    // This will have to be expanded to handle load from depth buffers aswell
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_index, texture_id, mip
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
        iss << data.id << ": " << data.name << "(" << data.width << "x" << data.height << ")";
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