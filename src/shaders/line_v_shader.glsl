#version 330 core
layout(location = 0) in vec3 position;

out VertexData{
    vec4 mColor;
} VertexOut;

uniform mat4 MVP;

void main() {
    vec4 temp = vec4(0, 1, 0, 1);
    VertexOut.mColor = temp;
    gl_Position = MVP * vec4(position, 1);
}