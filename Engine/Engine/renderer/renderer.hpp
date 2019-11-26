#ifndef _RENDERER_HPP
#define _RENDERER_HPP
#include <memory>
#include "glm/glm.hpp"

#include "Engine/transform.hpp"
#include "Engine/event/event.hpp"
#include "Engine/event/bus.hpp"
#include "Engine/renderer/renderer_api.hpp"
#include "Engine/renderer/camera.hpp"
#include "Engine/renderer/shader.hpp"
#include "Engine/entity.hpp"
#include "Tools/gltf_loader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace engine {

struct SceneData {
    glm::vec4 clear_color;
    int width;
    int height;
};

class Renderer {
public:

    static void begin_scene(const std::shared_ptr<Camera> camera, const SceneData data);
    static void end_scene();

    static void on_ui_render(bool draw);

    static void update_display(int width, int height);

    static void submit(
        const std::shared_ptr<Shader>& shader,
        const std::shared_ptr<engine::VertexArray>& vertex_array,
        const glm::mat4& model
    );

    static void submit_entity(
        const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity,
        Transform &transform
    );

    inline static RendererAPI::API get_api() { return RendererAPI::get_api(); }

    struct SceneState {
        glm::mat4 view_projection_matrix;
    };
    static SceneState* scene_state;
private:
    static RendererAPI* renderer_api;

    static void submit_node(
        const std::shared_ptr<Shader>& shader, std::shared_ptr<Entity> &entity, 
        NodeObject &node_object, glm::mat4 global_transform
    );
};

} //namespace
#endif