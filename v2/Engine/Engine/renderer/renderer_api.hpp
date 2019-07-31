#ifndef _RENDERER_API_HPP
#define _RENDERER_API_HPP
#include <memory>

#include <glm/glm.hpp>

#include "Engine/renderer/vertex_array.hpp"

namespace engine {

class RendererAPI {
public:
    enum class API
    {
        None = 0, OpenGL = 1, OpenGLCompat = 2
    };
public:
    virtual void clear(const glm::vec4 &color) = 0;

    virtual void draw_indexed(const std::shared_ptr<VertexArray>& vertex_array) = 0;

    inline static API get_api() { return api; }

private:
    static API api;
};

} //namespace
#endif