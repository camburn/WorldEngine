#version 330 core
in VertexData{
    vec2 mTexCoord;
    vec4 mColor;
} VertexIn;

out vec4 color;

void main() {
    color = VertexIn.mColor;
}