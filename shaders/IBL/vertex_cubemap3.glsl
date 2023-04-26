#version 450

layout(location=0) in vec4 position;

out vec3 f_localpos;

layout(location = 0) uniform mat4 u_view_projection;

void main() {
    f_localpos = position.xyz;
    gl_Position = u_view_projection * position;
}