#version 450

#define BIAS 0.01
in vec4 frag_pos;

out vec4 out_color;

void main() {
    float value = gl_FragCoord.z;
    value += gl_FrontFacing ? BIAS: 0.0;
    out_color = vec4(value);
}