#include "engine.hpp"
#include "gl_texture.hpp"

namespace enginegl {

GLuint buffer_image(tinygltf::Sampler &sampler, tinygltf::Image &image) {
    //glActiveTexture(GL_TEXTURE0);
    //glUniform1i(glGetUniformLocation(ourShader.ID, "base_color"), 0);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
    return texture_id;
}

}