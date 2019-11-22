#include "engine.hpp"
#include "generate_sphere.hpp"

namespace engine {

std::shared_ptr<CustomEntity> generate_sphere() {
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec2> uv;
    std::vector<uint32_t> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec4(xPos, yPos, zPos, 1.0f));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec4(xPos, yPos, zPos, 0.0f));
        }
    }

    bool oddRow = false;
    for (int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        } else {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    uint32_t indexCount = indices.size();

    std::shared_ptr<CustomEntity> entity( new CustomEntity(DrawMode::TRIANGLE_STRIP));

    entity->add_attribute_data("position", positions);
    entity->add_attribute_data("normal", normals);
    entity->add_attribute_data("texcoord", uv);
    entity->add_index_data(indices);
    entity->name = "Sphere";

    entity->add_uniform_data("u_model", glm::mat4(1.0f) );

    return entity;
}

}
