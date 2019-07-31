#ifndef _RENDERER_HPP
#define _RENDERER_HPP

#include "Engine/renderer/renderer_api.hpp"

namespace engine {

class Renderer {
public:
    static void begin_scene(const float r, const float g, const float b, const float a);
    static void begin_scene(const glm::vec4& color);
    static void end_scene();

    static void submit(const std::shared_ptr<engine::VertexArray>& vertex_array);

    inline static RendererAPI::API get_api() { return RendererAPI::get_api(); }
private:
    static RendererAPI* renderer_api;
};

} //namespace
#endif