#version 120

varying vec3 f_normal;
varying vec4 f_fragpos;
varying vec2 f_texcoord;

uniform sampler2D albedo;
uniform sampler2D normal;

// Roughness Metallic Ambient Occlusion map
// Ambient Occlusion from R channel
// Roughness from G channel
// Metallic from B channel
uniform sampler2D ambient_roughness_metallic;


uniform vec3 u_lightpos = vec3(20, 5, 20);
uniform vec3 u_lightcolor = vec3(1, 1, 1);
uniform vec4 u_color;
uniform float ambient_strength = 0.8;

const float PI = 3.14159265359;

void main() {
    
    //vec3 albedo_sample = pow(texture2D(albedo, f_texcoord).rgb, vec3(2.2)); // sRGB Converter
    vec3 albedo_sample = texture2D(albedo, f_texcoord).rgb;
    float ambient = texture2D(ambient_roughness_metallic, f_texcoord).r;
    float roughness = texture2D(ambient_roughness_metallic, f_texcoord).g;
    float metallic = texture2D(ambient_roughness_metallic, f_texcoord).b;

    //vec3 ambient = ambient_strength * u_lightcolor;
    vec3 norm = normalize(f_normal);
    vec3 lightdir = normalize(u_lightpos - f_fragpos.xyz);  
    float diff = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = diff * u_lightcolor;

    vec3 result = (ambient + diffuse) * albedo_sample;

    gl_FragColor = vec4(result, 1);
}