#version 120

attribute vec4 position;

varying vec3 f_localpos;

uniform mat4 u_view_projection;

void main() {
    f_localpos = position.xyz;
    gl_Position = u_view_projection * position;
}