#ifndef _GL_CONTEXT_HPP
#define _GL_CONTEXT_HPP

#include "Engine/renderer/context.hpp"

struct GLFWwindow;

namespace enginegl {

void _check_gl_error(const char *file, int line);

class OpenGLContext: public GraphicsContext {
public:
    OpenGLContext(GLFWwindow* window_handle);

    virtual void init() override;

    virtual void swap_buffers() override;

private:
    GLFWwindow* window_handle;
};

#define check_gl_error() enginegl::_check_gl_error(__FILE__,__LINE__)

} //namespace
#endif