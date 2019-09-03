#version 120

attribute vec4 position;
attribute vec4 normal;
attribute vec2 texcoord;

uniform mat4 u_view_projection;
uniform mat4 u_model;

varying vec3 f_normal;
varying vec3 f_worldpos;
varying vec2 f_texcoord;

void main() {
    f_texcoord = texcoord;
    f_normal = (u_model * normal).xyz;
    f_worldpos = (u_model * position).xyz;
    gl_Position = u_view_projection * u_model * position;
}