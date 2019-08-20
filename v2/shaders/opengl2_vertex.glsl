#version 120

attribute vec4 position;
attribute vec4 normal;
attribute vec2 texcoord;

uniform mat4 u_view_projection;
uniform mat4 u_model;

varying vec3 f_normal;
varying vec4 f_fragpos;
varying vec2 f_texcoord;

void main() {
    f_texcoord = texcoord;
    f_normal = normal.xyz;
    f_fragpos = u_model * position;
    gl_Position = u_view_projection * u_model * position;
}