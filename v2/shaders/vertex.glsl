#version 460

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 texcoord;

uniform mat4 u_view_projection;
uniform mat4 u_model;

out vec3 f_normal;
out vec4 f_fragpos;
out vec2 f_texcoord;

void main() {
    f_texcoord = texcoord;
    f_normal = normal.xyz;
    f_fragpos = u_model * position;
    gl_Position = u_view_projection * u_model * position;
}