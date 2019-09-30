#version 460

layout(location=0) in vec4 position;
layout(location=1) in vec2 texcoord;

out vec2 f_texcoord;


void main() {
    f_texcoord = texcoord;
    gl_Position = position;
}