#ifndef _GL_DEBUG_DRAW_HPP
#define _GL_DEBUG_DRAW_HPP

#include <glm/glm.hpp>
#include "glad/glad.h"
#include "Platform/OpenGL/gl_buffer.hpp"
#include "Platform/OpenGL/gl_vertex_array.hpp"


namespace enginegl {

const float LINE_WIDTH = 0.05f;

std::vector<uint32_t> elements {
    0, 2, 1,
    0, 3, 2,

    0, 1, 4,
    5, 4, 1,
    3, 0, 7,
    4, 7, 0,
    2, 3, 6,
    7, 6, 3,
    1, 2, 5,
    6, 5, 2,

    7, 4, 6,
    6, 4, 5,
};


void gl_draw_line(glm::vec3 a, glm::vec3 b) {

    glm::vec3 dir = glm::normalize(b - a);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(dir, up));

    std::vector<glm::vec4> vertices {
        glm::vec4(a - up * LINE_WIDTH - right * LINE_WIDTH, 1.0f ),
        glm::vec4(a + up * LINE_WIDTH - right * LINE_WIDTH, 1.0f ),
        glm::vec4(a + up * LINE_WIDTH + right * LINE_WIDTH, 1.0f ),
        glm::vec4(a - up * LINE_WIDTH + right * LINE_WIDTH, 1.0f ),

        glm::vec4(b - up * LINE_WIDTH - right * LINE_WIDTH, 1.0f ),
        glm::vec4(b + up * LINE_WIDTH - right * LINE_WIDTH, 1.0f ),
        glm::vec4(b + up * LINE_WIDTH + right * LINE_WIDTH, 1.0f ),
        glm::vec4(b - up * LINE_WIDTH + right * LINE_WIDTH, 1.0f )
    };

    auto buffer = OpenGLVertexBuffer(
        &vertices[0].x, vertices.size() * (sizeof(float) * 4)
    );
    auto i_buffer = OpenGLIndexBuffer(&elements[0], elements.size());
    buffer.bind();
    i_buffer.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (const void*)0
    );
    glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
}

}

#endif