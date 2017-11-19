#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in uint model_loc;

//uniform mat4 MVP;
uniform mat4 View;
uniform mat4 Projection;
//uniform mat3 NormalMat;

layout(std140, binding=1) uniform ModelMatrices {
    mat4 model[16];
};

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; // In World Space

void main(){
    mat4 m = model[model_loc];
    mat3 normal_mat = transpose(inverse(mat3(m)));
    TexCoord = texCoord;
	Normal = normal_mat * normal;
	FragPos = vec3(m * vec4(position, 1.0f));
    gl_Position = Projection * View * m * vec4(position, 1);
    // More efficient way (precalculate MVP)
    //gl_Position = mvp * vec4(position, 1);
}