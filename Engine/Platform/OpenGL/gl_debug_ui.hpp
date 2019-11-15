#ifndef _GL_DEBUG_UI_HPP
#define _GL_DEBUG_UI_HPP

#include "Platform/OpenGL/gl_texture.hpp"

namespace enginegl {

void on_ui_render(bool display) {
    display_loaded_textures(display);
}

}

#endif