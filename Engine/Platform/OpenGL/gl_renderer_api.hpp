#ifndef _GL_RENDERER_API_HPP
#define _GL_RENDERER_API_HPP
#include <memory>

#include <glm/glm.hpp>

#include "Engine/renderer/vertex_array.hpp"
#include "Engine/renderer/renderer_api.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace glengine {

class GLRendererAPI: public engine::RendererAPI {
public:
    virtual void clear(const glm::vec4 &color) override;

    virtual void update_screen(int width, int height) override;

    virtual void cull_face_front(bool enable);

    virtual void map_texture(const unsigned int tex_id) override;
    virtual void map_texture_unit(const unsigned int tex_id, const unsigned int unit) override;

    virtual void draw_indexed(const std::shared_ptr<engine::VertexArray>& vertex_array) override;
    virtual void draw_triangles(const std::shared_ptr<engine::VertexArray>& vertex_array) override;
};

} //namespace
#endif