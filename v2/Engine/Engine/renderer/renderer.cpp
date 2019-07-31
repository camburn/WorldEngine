#include "engine.hpp"

#include "glm/glm.hpp"

#include "renderer.hpp"

namespace engine {


void Renderer::begin_scene(const float r, const float g, const float b, const float a) {
    begin_scene(glm::vec4(r, g, b, a));
}

void Renderer::begin_scene(const glm::vec4& color) {
    renderer_api->clear(glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
}

void Renderer::end_scene() {

}

void Renderer::submit(const std::shared_ptr<engine::VertexArray>& vertex_array) {
    renderer_api->draw_indexed(vertex_array);
}

} //namespace