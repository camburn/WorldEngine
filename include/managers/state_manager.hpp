#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "graphics/renderer.hpp"
#include "graphics/debug.hpp"


#ifndef STATE_H
#define STATE_H

class Light {
public:
    Light() : position(glm::vec3(1.0f)), light_color(glm::vec3(1.0f)), intensity(1.0f) {}
    Light(glm::vec3 position) : position(position), light_color(glm::vec3(1.0f)), intensity(1.0f) {}
    const std::string type = "Light";

    void update_position(glm::vec3 new_pos);

protected:

    glm::vec3 position;
    glm::vec3 light_color;
    float intensity;
};

class DirectionLight : public Light {
public:
    DirectionLight() : Light() {}
    const std::string type = "DirectionLight";

protected:
};

class PointLight : public Light {
public:
    PointLight() : Light() {}
    PointLight(glm::vec3 position) : Light(position) {}
    const std::string type = "PointLight";

    void generate_light_matrix();

protected:
    const int cube_sides {6};
    glm::mat4 cube_map[6];

};

class State {
    // Represents the world state of a single shader and camera
public:
    State(Renderer &state_renderer);
    State(Renderer &state_renderer, glm::mat4 &proj_mat, glm::mat4 &view_mat);
    GLuint get_program();

    void set_projection(glm::mat4 matrix);
    void set_view(glm::mat4 matrix);

    void set_light_pos(glm::vec3 pos);
    void set_view_pos(glm::vec3 pos);

    void update_state();

    void set_direction_light(glm::vec3 pos);
    void create_point_light(glm::vec3 pos);
    void update_point_light(int index, glm::vec3 pos);

    glm::mat4 generate_light_matrix();
    glm::mat4 generate_model_view();
    Renderer renderer;

private:
    GLuint program_id = -1;
    bool invalid_model_view = true;
    glm::mat4 cached_model_view {1.0f};
    glm::mat4 projection_matrix {1.0f};
    glm::mat4 view_matrix {1.0f};

    glm::vec3 view_pos {1.0f};
    glm::vec3 light_pos {1.0f};
    std::vector<PointLight> point_lights;
    DirectionLight direction_light;   // Currently only support 1 light
};

void show_shadow_map_settings(bool* p_open);
#endif 