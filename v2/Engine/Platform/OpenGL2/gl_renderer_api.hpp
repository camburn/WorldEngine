#ifndef _GL_RENDERER_API_HPP
#define _GL_RENDERER_API_HPP
#include <memory>

#include <glm/glm.hpp>

#include "Engine/renderer/vertex_array.hpp"
#include "Engine/renderer/renderer_api.hpp"

namespace glengine {

class GL2RendererAPI: public engine::RendererAPI {
public:
    virtual void clear(const glm::vec4 &color) override;

    virtual void draw_indexed(const std::shared_ptr<engine::VertexArray>& vertex_array) override;
};

} //namespace
#endif