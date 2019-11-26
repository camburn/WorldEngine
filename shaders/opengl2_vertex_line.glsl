#version 120

attribute vec4 position;
attribute vec4 color;

varying vec4 u_color;

uniform mat4 u_view_projection;
uniform mat4 u_model;


void main() {

    u_color = color;
    gl_Position = u_view_projection * u_model * position;
}