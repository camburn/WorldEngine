#version 120

#define BIAS 0.01

void main() {
    gl_FragDepth = gl_FragCoord.z;
    gl_FragDepth += gl_FrontFacing ? BIAS: 0.0;
}