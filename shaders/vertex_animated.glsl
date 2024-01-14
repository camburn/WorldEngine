#version 450

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 texcoord;
layout(location=3) in vec4 weights_0;
layout(location=4) in vec4 joints_0;

uniform mat4 u_view_projection;
uniform mat4 u_model;
uniform mat4 u_light_space_matrix;

layout(std430, binding = 0) buffer animation_data
{
    mat4 bind_matrices[];
};

out vec3 f_normal;
out vec3 f_worldpos;
out vec2 f_texcoord;
out vec4 f_frag_pos_light_space;

mat4 skin_matrix() {
    return weights_0.x * bind_matrices[int(joints_0.x)] +
        weights_0.y * bind_matrices[int(joints_0.y)] +
        weights_0.z * bind_matrices[int(joints_0.z)] +
        weights_0.w * bind_matrices[int(joints_0.w)];
}

void main() {
    f_normal = (mat3(u_model) * normal.xyz);

    f_texcoord = texcoord;
    
    f_worldpos = (u_model * vec4(position.xyz, 1)).xyz;
    f_frag_pos_light_space = u_light_space_matrix * vec4(f_worldpos, 1);
    gl_Position = u_view_projection * u_model * skin_matrix() * position;
}