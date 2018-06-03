#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "graphics/renderer.hpp"
#include "graphics/debug.hpp"


#ifndef STATE_H
#define STATE_H
class State {
    // Represents the world state of a single shader and camera
public:
    State(Renderer &state_renderer);
    GLuint get_program();

    void set_projection(glm::mat4 &matrix);
    void set_view(glm::mat4 &matrix);

    void set_light_pos(glm::vec3 pos);
    void set_view_pos(glm::vec3 pos);

    void update_state();

    glm::mat4 generate_model_view();
    Renderer renderer;

private:
    GLuint program_id;
    bool invalid_model_view = true;
    glm::mat4 cached_model_view;
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;

    glm::vec3 view_pos;
    glm::vec3 light_pos;
};
#endif 