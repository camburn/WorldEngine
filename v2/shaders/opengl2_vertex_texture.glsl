#version 120

attribute vec4 position;
attribute vec4 normal;
attribute vec2 texcoord;

uniform mat4 u_view_projection;
uniform mat4 u_model;
uniform mat4 u_light_space_matrix;

varying vec3 f_normal;
varying vec3 f_worldpos;
varying vec2 f_texcoord;
varying vec4 f_frag_pos_light_space;

void main() {
    f_texcoord = texcoord;
    f_normal = (u_model * normal).xyz;
    f_worldpos = (u_model * vec4(position.xyz, 1)).xyz;
    f_frag_pos_light_space = u_light_space_matrix * vec4(f_worldpos, 1);
    gl_Position = u_view_projection * u_model * position;
}