#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 light_matrix;
uniform mat4 model;

void main(){

    gl_Position = light_matrix * model * vec4(position, 1);
}