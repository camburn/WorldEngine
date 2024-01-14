#version 450

in vec3 f_localpos;
out vec4 out_color;
uniform samplerCube environment_map;

void main() {
    vec3 color = texture(environment_map, f_localpos).rgb;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    out_color = vec4(color, 1);
}