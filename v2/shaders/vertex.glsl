#version 330 core

layout(location=0) in vec4 position;
layout(location=1) in vec4 color;

uniform mat4 u_view_projection;
uniform mat4 u_model;

out vec4 f_color;
out vec4 f_position;

void main() {
    f_color = color;
    f_position = position;
    gl_Position = u_view_projection * u_model * position;
}