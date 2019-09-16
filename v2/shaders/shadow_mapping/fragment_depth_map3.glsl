#version 460

#define BIAS 0.01

out vec4 out_color;

void main() {
    out_color = gl_FragCoord.z;
    out_color += gl_FrontFacing ? BIAS: 0.0;
}