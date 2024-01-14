#version 450

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 texcoord;

/*
layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
*/

uniform mat4 u_view_projection;
uniform mat4 u_model;
uniform mat4 u_light_space_matrix;

out vec3 f_normal;
out vec3 f_worldpos;
out vec2 f_texcoord;
out vec4 f_frag_pos_light_space;

void main() {
    f_normal = (mat3(u_model) * normal.xyz);

    f_texcoord = texcoord;
    
    f_worldpos = (u_model * vec4(position.xyz, 1)).xyz;
    f_frag_pos_light_space = u_light_space_matrix * vec4(f_worldpos, 1);
    gl_Position = u_view_projection * u_model * position;// *projection;
}