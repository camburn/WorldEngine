#version 120

uniform vec4 u_color;

void main() {
    gl_FragColor = vec4(u_color.xyz, 1);
}