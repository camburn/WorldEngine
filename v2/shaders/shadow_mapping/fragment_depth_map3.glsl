#version 460

#define BIAS 0.01

out vec4 out_color;

void main() {
    float value = gl_FragCoord.z;
    value += gl_FrontFacing ? BIAS: 0.0;
    out_color = vec4(value);
}