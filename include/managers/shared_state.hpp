#include <vector>
#include <string>
#include <glm/glm.hpp>

#ifndef SHARED_STATE_H
#define SHARED_STATE_H

/*
Idea is the following:
> frame_start
> set all uniform state
> Buffer all state
> Render frame
> frame_end
*/

class InstanceUniforms {
public:
    glm::mat4 model_matrix;
    glm::mat4 normal_matrix;
    glm::mat4 mvp_matrix;  // model view perpective matrix
    glm::vec3 object_color;
    float diffuse_texture_sampler_id;
    float use_shadows;
    float use_point_shadow;
    float use_direction_shadow;
};

class SharedState {

private:
    struct Common {
        // Camera
        glm::mat4 perspective_matrix;
        glm::mat4 view_matrix;
        glm::vec3 view_pers_matrix;  // view perspective matrix
        float far_plane;
        
        // Lights
        // Direction Light
        glm::mat4 light_direction_projection_matrix;
        glm::mat4 light_direction_view_matrix;
        glm::mat4 light_direction_vp_matrix;  // view perspective matrix
        glm::vec3 light_direction_pos;
        glm::vec3 light_direction_color;

        // Point Light
        glm::vec3 light_point_pos;
        glm::vec3 light_point_color;

        // Shadows
        float direction_shadow_map_sampler_id;
        float point_shadow_cube_map_sampler_id;
        float pcf_samples;
        float shadow_map_bias;
        float cube_map_bias;

        // Debug flags
        float debug_draw_normals;
        float debug_draw_texcoords;
        float debug_draw_lighting;

        // Arrays

        // Only the last array can be dynamic size in OpenGL
        std::vector<InstanceUniforms> instance_uniforms;
    };

    struct DepthMapState {
        bool cube_matrix;
        glm::vec3 light_pos;
        float far_plane;
        glm::mat4 light_matrix;
        glm::mat4 light_cube_matrix[8];
    };

public:
    SharedState() {};
    Common common;

};

#endif