#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef DATA_MODELS_H
#define DATA_MODELS_H

#define MAX_POINT_LIGHTS 10

class PointLightModel {
public:
    glm::vec4 position {1.0f};

    glm::vec4 ambience {0.05f};
    glm::vec4 diffuse {0.8f};
    glm::vec4 specular {1.0f};

    glm::vec4 attributes {1.0f, 0.09f, 0.032f, 1.0f};
    // x = constant
    // y = linear
    // z = quadratic
};

class DirectionLightModel {
public:
    glm::vec4 direction {0.2f, -1.0f, -0.3f, 1.0f};
    glm::vec4 ambient {0.05f};
    glm::vec4 diffuse {0.4f};
    glm::vec4 specular {0.5f};
};

class Material {
public:
    glm::vec3 base_color {1};
    GLfloat shininess {32.0f};
    GLuint diffuse_set {1};
    GLuint specular_set {0};
    GLint diffuse_texture {0};
    GLint specular_texture {1};
};

class InstanceUniforms {
public:
    glm::mat4 model_matrix {0.0f};
    glm::mat4 normal_matrix {0.0f};
    glm::mat4 mvp_matrix {0.0f};  // model view perpective matrix
    glm::vec3 object_color {0.0f};
    GLfloat diffuse_texture_sampler_id {0.0f};
    GLfloat use_shadows {0.0f};
    GLfloat use_point_shadow {0.0f};
    GLfloat use_direction_shadow {0.0f};
    GLfloat _padding[1] {-99.0};
};

class SharedState{
// **Warning** This structure must match the shader version exactly
public:
    glm::uvec4 debug_flags {0};
    // x = s_debug_draw_normals
    // y = s_debug_draw_texcoords
    // z = s_debug_draw_lighting
    // w = UNUSED

    glm::uvec4 shadow_flags {1};
    // x = s_use_point_shadows
    // y = s_use_direction_shadow
    // z = s_pcf_samples
    // w = UNUSED

    glm::vec4 map_config {0.0005f, 0.0005f, 0.0f, 0.0f};
    // x = s_shadow_map_bias
    // y = s_cube_map_bias
    // z = s_direction_shadow_map_sampler_id
    // w = s_point_shadow_cube_map_sampler_id

    glm::vec4 map_ranges {25.0f, 0.0f, 0.0f, 0.0f};
    // x = s_far_plane
    // y = UNUSED
    // z = UNUSED
    // w = UNUSED

    glm::mat4 perspective_matrix {0.0f};
    glm::mat4 view_matrix {0.0f};
    glm::mat4 light_direction_projection_matrix {0.0f};
    glm::mat4 light_direction_view_matrix {0.0f};
    glm::mat4 light_direction_vp_matrix {0.0f}; // view pers matrix 
    glm::vec4 view_pers_matrix {0.0f};
    glm::vec4 view_pos {0.0f};
    glm::vec4 light_direction_pos {0.0f};
    glm::vec4 light_direction_color {0.0f};
    glm::vec4 light_point_pos {0.0f};
    glm::vec4 light_point_color {0.0f};

    glm::uvec4 state_flags {0};
    // x = _render_flag
    // y = _debug_flag
    // z = point_light_count
    // w = UNUSED

    DirectionLightModel direction_light;
};

class LightState {
public:
    PointLightModel point_lights[MAX_POINT_LIGHTS];
};
#endif