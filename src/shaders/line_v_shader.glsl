#version 330 core
layout(location = 0) in vec3 position;
//out vec3 v_position;

uniform mat4 MVP;

void main() {
    //v_position = position;
    gl_Position = MVP * vec4(position, 1);
}