#version 450

layout(location=0) in vec4 position;

uniform mat4 u_model;
uniform mat4 u_view_projection;

void main() {
    gl_Position = u_view_projection * u_model * position;
}