#include "engine.hpp"
#include <vector>
#include "glm/glm.hpp"

#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"

namespace engine {

std::shared_ptr<VertexArray> generate_sphere() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
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

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y       * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y       * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    uint32_t indexCount = indices.size();

    auto cube_vao = VertexArray::create();
    auto cube_vbo = VertexBuffer::create(&positions.at(0), sizeof(positions));
    auto cube_ibo = IndexBuffer::create(&indices.at(0), indices.size());

    cube_vbo->set_layout({
        { engine::ShaderDataType::Float3, "position" },
        { engine::ShaderDataType::Float3, "normal" },
        { engine::ShaderDataType::Float3, "texcoord" },
    });
    cube_vao->add_vertex_buffer(cube_vbo);
    cube_vao->set_index_buffer(cube_ibo);
    return cube_vao;
}

}