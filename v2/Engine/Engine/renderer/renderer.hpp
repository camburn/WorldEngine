#ifndef _RENDERER_HPP
#define _RENDERER_HPP
#include <memory>
#include "glm/glm.hpp"

#include "Engine/renderer/renderer_api.hpp"
#include "Engine/renderer/camera.hpp"
#include "Engine/renderer/shader.hpp"
#include "Engine/entity.hpp"
#include "Tools/gltf_loader.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace engine {

class Renderer {
public:

    static void begin_scene(const std::shared_ptr<Camera> camera, const glm::vec4& clear_color);
    static void end_scene();

    static void on_ui_render(bool draw);

    static void submit(
        const std::shared_ptr<Shader>& shader,
        const std::shared_ptr<engine::VertexArray>& vertex_array,
        const glm::mat4& model
    );

    static void submit_entity(const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity);

    inline static RendererAPI::API get_api() { return RendererAPI::get_api(); }
private:
    struct SceneData {
        glm::mat4 view_projection_matrix;
    };
    static SceneData* scene_data;
    static RendererAPI* renderer_api;

    static void submit_node(
        const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity, 
        NodeObject &node_object, glm::mat4 global_transform
    );
};

} //namespace
#endif