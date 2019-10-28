#version 450

layout(location=0) in vec4 position;

uniform mat4 u_view_projection;
uniform mat4 u_model;

void main() {

    gl_Position = u_view_projection * u_model * position;
}