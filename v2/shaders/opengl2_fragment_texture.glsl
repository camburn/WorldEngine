#version 120

const int MAX_LIGHTS = 2;
const float PI = 3.14159265359;

varying vec3 f_normal;
varying vec3 f_worldpos;
varying vec2 f_texcoord;

uniform sampler2D albedo;
uniform sampler2D normal;

// Roughness Metallic Ambient Occlusion map
// Ambient Occlusion from R channel
// Roughness from G channel
// Metallic from B channel
uniform sampler2D ambient_roughness_metallic;


// First light is always directional
uniform float ambient_strength = 0.8;

uniform vec3 u_camera_position;
uniform vec3 u_lightpos[MAX_LIGHTS];
uniform vec3 u_lightcolor[MAX_LIGHTS];

vec3 get_normal_from_map() {
    vec3 tangent_normal = texture2D(normal, f_texcoord).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(f_worldpos);
    vec3 Q2 = dFdy(f_worldpos);
    vec3 st1 = dFdx(f_worldpos);
    vec3 st2 = dFdy(f_worldpos);

    vec3 N = normalize(f_normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_normal);
}

float distribution_ggx(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);

    denom = PI * denom * denom;
    return nom / denom;
}

float geometry_schlick_ggx(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_ggx(NdotV, roughness);
    float ggx1 = geometry_schlick_ggx(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cos_theta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}

void main() {
    
    //vec3 albedo_sample = pow(texture2D(albedo, f_texcoord).rgb, vec3(2.2)); // sRGB Converter
    vec3 albedo_sample = texture2D(albedo, f_texcoord).rgb;
    float ambient_occlusion = texture2D(ambient_roughness_metallic, f_texcoord).r;
    float roughness = texture2D(ambient_roughness_metallic, f_texcoord).g;
    float metallic = texture2D(ambient_roughness_metallic, f_texcoord).b;

    vec3 N = get_normal_from_map();
    vec3 V = normalize(u_camera_position - f_worldpos);

    vec3 F0 = vec3(0.4);
    F0 = mix(F0, albedo_sample, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < u_lightpos.length(); i++) {  // Calculate per light radiance
        vec3 L = normalize(u_lightpos[i] - f_worldpos);
        vec3 H = normalize(V + L);
        float distance = length(u_lightpos[i] - f_worldpos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance;
        if (i == 0) {
            radiance = u_lightcolor[i];
        } else {
            //Attenuation is for a point light
            vec3 radiance = u_lightcolor[i] * attenuation;
        }
        // Not using it results in a directional light

        //Cook-Torrance BRDF
        float NDF = distribution_ggx(N, H, roughness);
        float G = geometry_smith(N, V, L, roughness);
        vec3 F = fresnel_schlick(max(dot(H, V), 0.0), F0);
        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;  // To prevent divide by zero
        vec3 specular = nominator / denominator;

        // kS is equal to fresnel
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * albedo_sample / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo_sample * ambient_occlusion;

    vec3 color = ambient * Lo;
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    gl_FragColor = vec4(color, 1);
}