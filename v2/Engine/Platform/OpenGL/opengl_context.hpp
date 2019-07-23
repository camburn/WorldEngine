#ifndef _GL_CONTEXT_HPP
#define _GL_CONTEXT_HPP

#include "Engine/renderer/context.hpp"

class GLFWwindow;

namespace enginegl {

class OpenGLContext: public GraphicsContext {
public:
    OpenGLContext(GLFWwindow* window_handle);

    virtual void init() override;

    virtual void swap_buffers() override;

private:
    GLFWwindow* window_handle;
};

} //namespace
#endif