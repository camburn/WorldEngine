#include "engine.hpp"
#include "gl_draw_debug.hpp"

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

/*
Draw a line from a cube

a Get a 1x1x1 cube
Get a forward vector i.e. v3(0, 1, 0)

Determine line direction vector (normalize(b - a))
then rotate cube to face direction

Then determine distance (glm::distance(a, b))
Then scale forward vector by distance
*/

static bool initialised = false;
static GLuint vbo;
static GLuint vao;

std::vector<glm::vec4> line_data;

void new_frame() {
    line_data.clear();
}

void draw_line(glm::vec4 a, glm::vec4 b, glm::vec4 color) {
    line_data.push_back(a);
    line_data.push_back(color);
    line_data.push_back(b);
    line_data.push_back(color);
}

void draw_buffers() {
    if (line_data.empty()) return;
    if (!initialised) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        initialised = true;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(
            0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)0
        );
        glEnableVertexAttribArray(1); // color
        glVertexAttribPointer(
            1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(4 * sizeof(float))
        );

        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, line_data.size() * sizeof(glm::vec4), &line_data[0].x, GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);

    glDrawArrays(GL_LINES, 0, line_data.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


}
