#version 130

attribute vec4 position;
attribute vec2 texcoord;

varying vec2 f_texcoord;


void main() {
    f_texcoord = texcoord;
    gl_Position = position;
}