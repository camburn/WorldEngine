#version 460

const int MAX_LIGHTS = 2;
const float PI = 3.14159265359;

in vec3 f_normal;
in vec3 f_worldpos;
in vec2 f_texcoord;

uniform sampler2D albedo;
uniform sampler2D normal;
// Roughness Metallic Ambient Occlusion map
// Ambient Occlusion from R channel
// Roughness from G channel
// Metallic from B channel
uniform sampler2D roughness_metallic;
uniform sampler2D ambient;
uniform sampler2D emission;

uniform vec3 u_camera_position;
uniform vec3 u_lightpos[MAX_LIGHTS];
uniform vec3 u_lightcolor[MAX_LIGHTS];

uniform int u_render_mode = 0;

out vec4 out_color;

vec3 get_normal_from_map() {
    vec3 tangent_normal = texture(normal, f_texcoord).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(f_worldpos);
    vec3 Q2 = dFdy(f_worldpos);
    vec2 st1 = dFdx(f_texcoord);
    vec2 st2 = dFdy(f_texcoord);

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

    vec4 albedo_sample = pow(texture2D(albedo, f_texcoord), vec4(2.2, 2.2, 2.2, 1)); // sRGB Converter
    //vec4 albedo_sample = texture(albedo, f_texcoord);
    float ambient_occlusion = texture(ambient, f_texcoord).r;
    float roughness = texture(roughness_metallic, f_texcoord).g;
    float metallic = texture(roughness_metallic, f_texcoord).b;
    vec3 emission_sample = texture2D(emission, f_texcoord).xyz;

    vec3 N = get_normal_from_map();
    vec3 V = normalize(u_camera_position - f_worldpos);

    vec3 F0 = vec3(0.4);
    F0 = mix(F0, albedo_sample.xyz, metallic);

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
            radiance = u_lightcolor[i] * attenuation;
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

        Lo += (kD * albedo_sample.xyz / PI + specular) * radiance * NdotL;
    }

    vec3 ambient_value = vec3(0.03) * albedo_sample.xyz * ambient_occlusion;

    vec3 color = ambient_value * Lo;
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    color += emission_sample.xyz;

    vec4 final_color = vec4(color, albedo_sample.a);

    // Debug modes
    if (u_render_mode == 1) final_color = albedo_sample;
    else if (u_render_mode == 2) final_color = vec4(vec3(metallic), 1);
    else if (u_render_mode == 3) final_color = vec4(vec3(roughness), 1);
    else if (u_render_mode == 4) final_color = vec4(vec3(ambient_occlusion), 1);
    else if (u_render_mode == 5) final_color = vec4(emission_sample, 1);
    else if (u_render_mode == 6) final_color = vec4(Lo, 1);
    else if (u_render_mode == 7) final_color = vec4(1); // lighting
    else if (u_render_mode == 8) final_color = vec4(1); // fresnel
    else if (u_render_mode == 9) final_color = vec4(1); // irradiance
    else if (u_render_mode == 10) final_color = vec4(1); // reflection
    else if (u_render_mode == 11) final_color = vec4(f_normal, 1); // normal
    else if (u_render_mode == 12) final_color = vec4(texture2D(normal, f_texcoord).xyz, 1); // texture_normal
    else if (u_render_mode == 13) final_color = vec4(f_texcoord, 0, 1); // texture_normal
    else if (u_render_mode == 14) final_color = vec4(vec3(albedo_sample.a), 1); // texture_normal
    
    out_color = final_color;
}