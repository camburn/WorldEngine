#version 120

attribute vec4 position;
attribute vec4 normal;

uniform mat4 u_view_projection;
uniform mat4 u_model;

varying vec3 f_normal;
varying vec4 f_fragpos;

void main() {
    f_normal = normal.xyz;
    f_fragpos = u_model * position;
    gl_Position = u_view_projection * u_model * position;
}