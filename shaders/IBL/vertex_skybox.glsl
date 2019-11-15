#version 120

attribute vec4 position;

varying vec3 f_localpos;

uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    f_localpos = position.xyz;
    mat4 rot_view = mat4(mat3(u_view));
    vec4 clip_pos = u_projection * rot_view * vec4(f_localpos, 1.0);
    gl_Position = clip_pos.xyww;
}