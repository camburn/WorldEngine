#version 120

uniform mat4 u_view_projection;
uniform mat4 u_model;

void main() {
    gl_Position = u_view_projection * u_model * gl_Vertex;
}
