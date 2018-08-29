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
    mat4 model_matrix;
    mat4 normal_matrix;
    mat4 mvp_matrix;
    vec3 object_color;
    float diffuse_texture_sampler_id;
    float use_shadows;
    float use_point_shadow;
    float use_direction_shadow;
};

layout(std430, binding=1) buffer shader_uniforms {
    mat4 perspective_matrix;
    mat4 view_matrix;
    vec3 view_pers_matrix;
    float far_plane;
    
    mat4 light_direction_projection_matrix;
    mat4 light_direction_view_matrix;
    mat4 light_direction_vp_matrix; // view pers matrix
    vec3 light_pos;
    vec3 light_color;

    vec3 light_point_pos;
    vec3 light_point_color;

    float direction_shadow_map_sampler_id;
    float point_shadow_cube_map_sampler_id;
    float pcf_samples;
    float shadow_map_bias;
    float cube_map_bias;

    float debug_draw_normals;
    float debug_draw_texcoords;
    float debug_draw_lighting;

    // Arrays
    InstanceUniforms instance_uniforms[];
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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir) {
    vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if (proj_coords.z > 1.0) { return 0.0; }
    proj_coords = proj_coords * 0.5 + 0.5;
    // Shadow biasing to remove shadow acne
    float bias = clamp(shadow_map_bias*tan(acos(dot(Normal, lightDir))), 0, 0.01);
    float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    //closest_depth = closest_depth * -2 / (25.0f - 1.0f) - (25.0f + 1.0f)/(25.0f - 1.0f);
    
    // PCF (shadow softening)
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(shadow_map,   0);
    for (int x = -pcf_samples; x <= pcf_samples; ++x) {
        for (int y = -pcf_samples; y <= pcf_samples; ++y) {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 0.5f : 0.0f;
        }
    }
    return shadow;
}

float ShadowCalculation(vec3 fragPos) {
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPointPos;
    // ise the fragment to light vector to sample from the depth map
    float closest_depth = texture(shadow_cube_map, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closest_depth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float current_depth = length(fragToLight);
    // test for shadows
    //float shadow = currentDepth - cube_map_bias > closestDepth ? 1.0 : 0.0;
    if (closest_depth + cube_map_bias < current_depth) {
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

    if (!use_shadows && use_uniform_color) {
        color = vec4(uniform_color, 1);
        return;
    }

    float specularStrength = 0.5f;
    float shininess = 32.0f;

    //Direction vector from light to fragment
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    // Calculate diffuse impact by getting dot product between lightDir and the surface normal
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    // Calc light direction vewctor and reflect vector
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    // Calculate Specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;  

    float direction_shadow = ShadowCalculation(FragPosLightSpace, lightDir);
    float point_shadow = ShadowCalculation(FragPos);
    
    vec3 light_final;
    if (use_point_shadow == true && use_direction_shadow == true) {
        light_final = (ambience_strength + (1.0 - clamp(direction_shadow + point_shadow, 0.0f, 1.0f)) * (diffuse + specular)) * lightColor;
    } else if (use_direction_shadow == true && use_point_shadow == false) {
        light_final = (ambience_strength + (1.0 - direction_shadow) * (diffuse + specular)) * lightColor;
    } else if (use_direction_shadow == false && use_point_shadow == true) {
        light_final = (ambience_strength + (1.0 - clamp(point_shadow, 0.0f, 1.0f)) * (diffuse + specular)) * lightColor;
    } else {
        light_final = vec3(1.0f);
    }

    if (debug_disable_lighting) {
        color = (texture(texture_diffuse1, TexCoord) * vec4(objectColor, 1.0));
        return;
    }
    if (debug_draw_texcoords) {
        color = vec4(TexCoord, 0.0f, 1.0f);
        return;
    }
    if (debug_draw_normals) {
        color = vec4(Normal, 1.0f);
        return;
    }
    if (use_uniform_color) {
          color = (vec4(uniform_color, 1) * vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
    } else {
        color = (texture(texture_diffuse1, TexCoord) + vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
    }
}