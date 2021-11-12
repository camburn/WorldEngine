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
        None = 0, OpenGL = 1
    };
public:
    virtual void clear(const glm::vec4 &color) = 0;

    virtual void update_screen(int width, int height) = 0;

    virtual void cull_face_front(bool enable) = 0;

    virtual void draw_indexed(const std::shared_ptr<VertexArray>& vertex_array) = 0;
    virtual void draw_triangles(const std::shared_ptr<VertexArray>& vertex_array) = 0;

    virtual void map_texture(const unsigned int tex_id) = 0;
    virtual void map_texture_unit(const unsigned int tex_id, const unsigned int unit) = 0;

    inline static API get_api() { return api; }

private:
    static API api;
};

} //namespace
#endif