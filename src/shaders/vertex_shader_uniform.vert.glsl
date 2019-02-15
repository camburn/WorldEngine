#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 MVP;
uniform mat4 light_matrix;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 NormalMat;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; // In World Space
out vec4 FragPosLightSpace;
out vec4 PositionB;

void main(){
    TexCoord = texCoord;
    Normal = NormalMat * normal;
    FragPos = vec3(Model * vec4(position, 1.0));
    FragPosLightSpace = light_matrix * vec4(FragPos, 1.0);
    // More efficient way (precalculate MVP)
    gl_Position = MVP * vec4(position, 1);
}