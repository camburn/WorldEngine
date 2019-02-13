#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef DATA_MODELS_H
#define DATA_MODELS_H

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

class SharedState {

public:

    // **Warning** This structure must match the shader version exactly
    // Shadow flags
    GLuint use_point_shadows {1};
    GLuint use_direction_shadow {1};
    GLuint debug_draw_normals {0};
    GLuint debug_draw_texcoords {0};

    // Camera
    glm::mat4 perspective_matrix {0.0f};
    glm::mat4 view_matrix {0.0f};

    glm::vec3 view_pers_matrix {0.0f};  // view perspective matrix
    GLuint debug_draw_lighting {0};

    glm::vec3 view_pos {0.0f};
    GLfloat far_plane {0.0f};
    
    // Lights
    // Direction Light
    glm::mat4 light_direction_projection_matrix {0.0f};
    glm::mat4 light_direction_view_matrix {0.0f};
    glm::mat4 light_direction_vp_matrix {0.0f};  // view perspective matrix

    glm::vec3 light_direction_pos {0.0f};
    GLuint pcf_samples {1};

    glm::vec3 light_direction_color {0.0f};
    GLfloat shadow_map_bias {0.0f};

    // Point Light
    glm::vec3 light_point_pos {0.0f};
    GLfloat cube_map_bias {0.0f};

    glm::vec3 light_point_color {0.0f};
    GLfloat direction_shadow_map_sampler_id {0.0f};

    GLfloat point_shadow_cube_map_sampler_id {0.0f};
    GLuint _padding_0 {99};
    GLuint _padding_1 {99};

    // Arrays

    // Only the last array can be dynamic size in OpenGL
    //std::vector<InstanceUniforms> instance_uniforms;
    //};

    struct DepthMapState {
        bool cube_matrix;
        glm::vec3 light_pos;
        float far_plane;
        glm::mat4 light_matrix;
        glm::mat4 light_cube_matrix[8];
    };

    SharedState() {};
};

#endif