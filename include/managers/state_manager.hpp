#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "graphics/renderer.hpp"
#include "graphics/debug.hpp"
#include "managers/data_models.hpp"

#ifndef STATE_H
#define STATE_H



class Light {
public:
    Light() : position(glm::vec3(1.0f)), light_color(glm::vec3(1.0f)), intensity(1.0f) {}
    Light(glm::vec3 position) : position(position), light_color(glm::vec3(1.0f)), intensity(1.0f) {}
    const std::string type = "Light";

    void update_position(glm::vec3 new_pos);
    glm::vec3 get_position() { return position; }

protected:

    glm::vec3 position = glm::vec3(1.0f);
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

    struct Data {
        vec3 position;

        float constant;
        float linear;
        float quadratic;

        vec3 ambience;
        vec3 diffuse;
        vec3 specular;

        float far_plane;
    };

    PointLight() : Light() {
        cube_map.reserve(cube_sides);
    }
    PointLight(glm::vec3 position) : Light(position) {
        cube_map.reserve(cube_sides);
    }
    const std::string type = "PointLight";

    std::vector<glm::mat4> generate_light_matrix();
    std::vector<glm::mat4> get_cube_map();
    const int cube_sides {6};
    float get_far_plane() {return far_plane;}

protected:
    std::vector<glm::mat4> cube_map;
    float far_plane {25.0f};
};

class State {
    // Represents the world state of a single shader and camera
public:
    State(Renderer &state_renderer);
    State(Renderer &state_renderer, glm::mat4 &proj_mat, glm::mat4 &view_mat);
    GLuint get_program();

    void set_projection(glm::mat4 matrix);
    void set_view(glm::mat4 matrix);

    glm::vec3 &get_light_pos();

    void set_light_pos(glm::vec3 pos);
    void set_view_pos(glm::vec3 pos);

    void update_state();

    void set_direction_light(glm::vec3 pos);
    void create_point_light(glm::vec3 pos);
    void update_point_light(int index, glm::vec3 pos);
    PointLight get_point_light(int index);

    void light_settings(bool* p_open);

    glm::mat4 generate_light_matrix();
    glm::mat4 generate_model_view();
    Renderer renderer;

    std::vector<PointLight> point_lights;

    bool animate_direction_light = false;
    bool use_point_shadow = true;
    bool use_direction_shadow = true;

private:
    SharedState state;
    unsigned int pcf_samples = 1;
    float shadow_map_bias = 0.00005f;
    float cube_map_bias = 0.05f;
    GLuint program_id = -1;
    bool invalid_model_view = true;
    glm::mat4 cached_model_view {1.0f};
    glm::mat4 projection_matrix {1.0f};
    glm::mat4 view_matrix {1.0f};

    glm::vec3 view_pos {1.0f};
    glm::vec3 light_pos {3.0f, 10.0f, 3.0f};
    
    DirectionLight direction_light;   // Currently only support 1 light
    
};

#endif 