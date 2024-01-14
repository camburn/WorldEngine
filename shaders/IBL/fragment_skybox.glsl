#version 120

varying vec3 f_localpos;
uniform samplerCube environment_map;

void main() {
    vec3 color = textureCube(environment_map, f_localpos).rgb;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    gl_FragColor = vec4(color, 1.0);
}