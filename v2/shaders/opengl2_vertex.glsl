#version 120

attribute vec4 position;

uniform mat4 u_view_projection;
uniform mat4 u_model;

varying vec4 f_color;
varying vec4 f_position;

void main() {
    f_color = vec4(0.3, 0.5, 0.7, 1.0);
    f_position = position;
    gl_Position = u_view_projection * u_model * position;
}