#version 430

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambience;
    vec3 diffuse;
    vec3 specular;

    float far_plane;
};

struct DirectionLight {
    vec3 position;

    vec3 ambience;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 base_color;
    float shininess;
    bool diffuse_set;
    bool specular_set;
    sampler2D diffuse;
    sampler2D specular;
};

struct InstanceUniforms {
    mat4 i_model_matrix;
    mat4 i_normal_matrix;
    mat4 i_mvp_matrix;
    vec3 i_object_color;
    float i_diffuse_texture_sampler_id;
    float i_use_shadows;
    float i_use_point_shadow;
    float i_use_direction_shadow;

    float _padding[1];
};

layout(std430, binding=3) buffer shader_uniforms {

    uint s_use_point_shadows;
    uint s_use_direction_shadow;
    uint s_debug_draw_normals;
    uint s_debug_draw_texcoords;

    mat4 s_perspective_matrix;
    mat4 s_view_matrix;

    vec3 s_view_pers_matrix;
    uint s_debug_draw_lighting;
    
    vec3 s_view_pos;
    float s_far_plane;
    
    mat4 s_light_direction_projection_matrix;
    mat4 s_light_direction_view_matrix;
    mat4 s_light_direction_vp_matrix; // view pers matrix

    vec3 s_light_direction_pos;
    uint s_pcf_samples;

    vec3 s_light_direction_color;
    float s_shadow_map_bias;

    vec3 s_light_point_pos;
    float s_cube_map_bias;

    vec3 s_light_point_color;
    float s_direction_shadow_map_sampler_id;

    float s_point_shadow_cube_map_sampler_id;
    uint _padding_0;
    uint _padding_1;

    // Arrays
    //InstanceUniforms instance_uniforms[];
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; // In world space
in vec4 FragPosLightSpace;

out vec4 color;

uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadow_map;
uniform samplerCube shadow_cube_map;
uniform vec3 objectColor;

//uniform bool debug_draw_texcoords = false;
//uniform bool debug_draw_normals = false;
//uniform bool debug_disable_lighting = false;

//uniform bool use_point_shadow = true;
//uniform bool use_direction_shadow = true;
uniform bool use_shadows = false;

uniform bool use_uniform_color = false;
uniform vec3 uniform_color = vec3( 1.0f, 1.0f, 1.0f);

//uniform int pcf_samples = 1;
//uniform float shadow_map_bias = 0.00005f;
//uniform float cube_map_bias = 0.05f;

uniform float ambience_strength = 0.2f;

uniform float far_plane;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir) {
    vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if (proj_coords.z > 1.0) { return 0.0; }
    proj_coords = proj_coords * 0.5 + 0.5;
    // Shadow biasing to remove shadow acne
    float map_bias = 0.00005;// = s_shadow_map_bias;
    float bias = clamp(map_bias*tan(acos(dot(Normal, lightDir))), 0, 0.01);
    float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    //closest_depth = closest_depth * -2 / (25.0f - 1.0f) - (25.0f + 1.0f)/(25.0f - 1.0f);
    
    // PCF (shadow softening)
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
    for (uint x = -s_pcf_samples; x <= s_pcf_samples; ++x) {
        for (uint y = -s_pcf_samples; y <= s_pcf_samples; ++y) {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 0.5f : 0.0f;
        }
    }
    return shadow;
}

float ShadowCalculation(vec3 fragPos) {
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - s_light_point_pos;
    // ise the fragment to light vector to sample from the depth map
    float closest_depth = texture(shadow_cube_map, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closest_depth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float current_depth = length(fragToLight);
    // test for shadows
    //float shadow = currentDepth - cube_map_bias > closestDepth ? 1.0 : 0.0;
    if (closest_depth + s_cube_map_bias < current_depth) {
        return 0.5f;
    }
    return 0.0f;
}

vec3 calc_dir_light() {
    return vec3(1.0f);
}

vec3 calc_point_light(PointLight light, vec3 frag_pos, vec3 normal, vec3 view_dir) {
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light.position - frag_pos);
    // Diffuse shading
    float diff = max(dot(norm, light_dir), 0.0);
    // Specular component
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    // Attenuation
    float light_distance = length(light.position - frag_pos);
    // Light fall off
    float attenuation = 1.0f / (light.constant + light.linear * light_distance +
                        light.quadratic * (light_distance * light_distance));

    // Combine
    vec3 ambient = light.ambience * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main(){

    //color = vec4(s_use_point_shadows, s_use_direction_shadow, 0, 1);
    //return;

    if (!use_shadows && use_uniform_color) {
        color = vec4(uniform_color, 1);
        _padding_0 = 0;
        return;
    }

    float specularStrength = 0.5f;
    float shininess = 32.0f;

    //Direction vector from light to fragment
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(s_light_direction_pos - FragPos);
    // Calculate diffuse impact by getting dot product between lightDir and the surface normal
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * s_light_direction_color;
    // Calc light direction vewctor and reflect vector
    vec3 viewDir = normalize(s_view_pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    // Calculate Specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * s_light_direction_color;

    float direction_shadow = ShadowCalculation(FragPosLightSpace, lightDir);
    float point_shadow = ShadowCalculation(FragPos);

    uint use_point_shadow = 1;
    uint use_direction_shadow = 1;
    
    vec3 light_final;
    if (use_point_shadow == 1 &&
        use_direction_shadow == 1) {
        light_final = (ambience_strength + (1.0 - clamp(direction_shadow + point_shadow, 0.0, 1.0))
                    * (diffuse + specular)) * s_light_direction_color;
        _padding_0 = 1;
    } else if (use_direction_shadow == 1 &&
               use_point_shadow == 0) {
        light_final = (ambience_strength + (1.0 - direction_shadow) * (diffuse + specular)) * s_light_direction_color;
        _padding_0 = 2;
    } else if (use_direction_shadow == 0 &&
               use_point_shadow == 1) {
        light_final = (ambience_strength + (1.0 - clamp(point_shadow, 0.0, 1.0)) * (diffuse + specular)) * s_light_direction_color;
        _padding_0 = 3;
    } else {
        light_final = vec3(1.0);
        _padding_0 = 4;
    }

    if (s_debug_draw_lighting == 1) {
        color = (texture(texture_diffuse1, TexCoord) * vec4(objectColor, 1.0));
        _padding_1 = 1;
        return;
    }
    if (s_debug_draw_texcoords == 1) {
        color = vec4(TexCoord, 0.0, 1.0);
        _padding_1 = 2;
        return;
    }
    if (s_debug_draw_normals == 1) {
        color = vec4(Normal, 1.0);
        _padding_1 = 3;
        return;
    }
    if (use_uniform_color) {
          color = (vec4(uniform_color, 1) * vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
    } else {
        color = (texture(texture_diffuse1, TexCoord) + vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
    }
}