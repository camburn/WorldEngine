#version 460

in vec3 f_normal;
in vec4 f_fragpos;

uniform vec3 u_lightpos = vec3(2, 1, 2);
uniform vec3 u_lightcolor = vec3(1, 1, 1);
uniform vec4 u_color;
uniform float ambient_strength = 0.1;

out vec4 out_color;

void main() {
    vec3 ambient = ambient_strength * u_lightcolor;
    vec3 norm = normalize(f_normal);
    vec3 lightdir = normalize(u_lightpos - f_fragpos.xyz);  
    float diff = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = diff * u_lightcolor;

    vec3 result = (ambient + diffuse) * u_color.xyz;

    out_color = vec4(result, 1);
}