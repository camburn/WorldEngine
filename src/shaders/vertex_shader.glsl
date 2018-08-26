#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;


struct InstanceUniforms {
    mat4 model_matrix;
    mat4 normal_matrix;
    mat4 mvp_matrix;
    vec3 object_color;
    uint diffuse_texture_sampler_id;
    bool use_shadows;
    bool use_point_shadow;
    bool use_direction_shadow;
};

layout(std430, binding=1) buffer shader_uniforms {
    mat4 perspective_matrix;
    mat4 view_matrix;
    vec3 view_pers_matrix;
    float far_plane;
    
    mat4 light_direction_projection_matrix;
    mat4 light_direction_view_matrix;
    mat4 light_direction_vp_matrix; // view pers matrix
    vec3 light_pos;
    vec3 light_color;

    vec3 light_point_pos;
    vec3 light_point_color;

    uint direction_shadow_map_sampler_id;
    uint point_shadow_cube_map_sampler_id;
    int pcf_samples;
    float shadow_map_bias;
    float cube_map_bias;

    bool debug_draw_normals;
    bool debug_draw_texcoords;
    bool debug_draw_lighting;

    // Arrays
    InstanceUniforms instance_uniforms[];
};

uniform int instance_index;

uniform mat4 MVP;
uniform mat4 light_matrix;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 NormalMat;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; // In World Space
out vec4 FragPosLightSpace;

void main(){
    TexCoord = texCoord;
    Normal = NormalMat * normal;
    FragPos = vec3(Model * vec4(position, 1.0f));
    FragPosLightSpace = light_matrix * vec4(FragPos, 1.0);
    // More efficient way (precalculate MVP)
    gl_Position = MVP * vec4(position, 1);
}