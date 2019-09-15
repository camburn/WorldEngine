#version 460

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 texcoord;

uniform mat4 u_view_projection;
uniform mat4 u_model;

out vec3 f_normal;
out vec3 f_worldpos;
out vec2 f_texcoord;

void main() {
    f_texcoord = texcoord;
    f_normal = (u_model * normal).xyz;
    f_worldpos = (u_model * position).xyz;
    gl_Position = u_view_projection * u_model * position;
}