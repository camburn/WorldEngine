#version 120

varying vec3 f_normal;
varying vec4 f_fragpos;
varying vec2 f_texcoord;

uniform sampler2D base_color;
uniform vec3 u_lightpos = vec3(20, 5, 20);
uniform vec3 u_lightcolor = vec3(1, 1, 1);
uniform vec4 u_color;
uniform float ambient_strength = 0.1;

void main() {
    vec3 ambient = ambient_strength * u_lightcolor;
    vec3 norm = normalize(f_normal);
    vec3 lightdir = normalize(u_lightpos - f_fragpos.xyz);  
    float diff = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = diff * u_lightcolor;

    vec3 result = (ambient + diffuse) * u_color.xyz * texture2D(base_color, f_texcoord).xyz;;

    gl_FragColor = vec4(result, 1);
}