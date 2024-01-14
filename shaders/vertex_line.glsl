#version 450

layout(location=0) in vec4 position;
layout(location=1) in vec4 color;

out vec4 u_color;

uniform mat4 u_view_projection;
uniform mat4 u_model;


void main() {

    u_color = color;
    gl_Position = u_view_projection * u_model * position;
}