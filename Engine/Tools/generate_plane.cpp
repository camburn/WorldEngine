#include "engine.hpp"
#include "generate_sphere.hpp"

namespace engine {

    std::shared_ptr<CustomEntity> generate_plane() {
        std::vector<glm::vec4> positions = {
            {-0.5f, 0.0f, -0.5f, 1.0f},
            {-0.5f, 0.0f, 0.5f, 1.0f},
            {0.5f, 0.0f, 0.5f, 1.0f},
            {0.5f, 0.0f, -0.5f, 1.0f}
        };
        std::vector<glm::vec4> normals = {
           {0.0f, 1.0f, 0.0f, 0.0f},
           {0.0f, 1.0f, 0.0f, 0.0f},
           {0.0f, 1.0f, 0.0f, 0.0f},
           {0.0f, 1.0f, 0.0f, 0.0f}
        };
        std::vector<glm::vec2> uv = {
           {0.0f, 0.0f},
           {1.0f, 0.0f},
           {1.0f, 1.0f},
           {0.0f, 1.0f}
        };
        std::vector<uint32_t> indices = {0, 1, 3, 2};

        uint32_t indexCount = indices.size();

        std::shared_ptr<CustomEntity> entity(new CustomEntity(DrawMode::TRIANGLE_STRIP));

        entity->add_attribute_data("position", positions);
        entity->add_attribute_data("normal", normals);
        entity->add_attribute_data("texcoord", uv);
        entity->add_index_data(indices);
        entity->name = "Plane";

        entity->add_uniform_data("u_model", glm::mat4(1.0f));

        return entity;
    }

}
