#version 430

#define MAX_POINT_LIGHTS 10

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; // In world space
in vec4 FragPosLightSpace;

out vec4 color;

struct PointLight {
    vec3 position;

    vec3 ambience;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    uint _padding;
};

struct DirectionLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 base_color;
    float shininess;
    uint diffuse_set;
    uint specular_set;
    sampler2D diffuse;
    //sampler2D specular;
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

    float _padding;
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

    uint point_light_count;

    DirectionLight direction_light;

    // Arrays
    
    PointLight point_lights[MAX_POINT_LIGHTS];
    //InstanceUniforms instance_uniforms[];
};

uniform sampler2D shadow_map;
uniform samplerCube shadow_cube_map;
uniform vec3 objectColor;

uniform bool use_shadows = false;

uniform bool use_uniform_color = false;
uniform vec3 uniform_color = vec3( 1.0, 1.0, 1.0);

uniform float ambience_strength = 0.2;

uniform float far_plane = 25.0;

uniform Material material;

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

vec3 calc_dir_light(DirectionLight light, vec3 normal, vec3 view_dir) {
    vec3 light_direction = normalize(-light.direction);
    // Calculate diffuse impact by getting dot product between lightDir and the surface normal
    float diff = max(dot(normal, light_direction), 0.0);
    // Calc light direction vewctor and reflect vector
    vec3 reflectDir = reflect(-light_direction, normal);
    // Calculate Specular component
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = vec3(0.0);
    if (material.specular_set == 1) {
        //specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    }
    //return (ambient * diffuse * specular);
    return (ambient * diffuse);
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
    vec3 specular = vec3(1.0);
    if (material.specular_set == 1) {
        //specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    }

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main(){

    if (!use_shadows && use_uniform_color) {
        color = vec4(uniform_color, 1);
        _padding_0 = 0;
        return;
    }
    if (s_debug_draw_lighting == 1) {
        color = (texture(material.diffuse, TexCoord) * vec4(objectColor, 1.0));
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

    float specularStrength = 0.5f;
    float shininess = 32.0f;

    vec3 view_dir = normalize(s_view_pos - FragPos);
    vec3 normal = normalize(Normal);
    vec3 light_direction = normalize(s_light_direction_pos - FragPos);

    vec3 light_result = calc_dir_light(direction_light, normal, view_dir);
    color = vec4(light_result, 1);
    for (int i = 0; i > point_light_count; i++) {
        PointLight point_light = point_lights[i];
        light_result += calc_point_light(point_light, normal, FragPos, view_dir);
    }

    float direction_shadow = ShadowCalculation(FragPosLightSpace, light_direction);
    float point_shadow = ShadowCalculation(FragPos);
    
    if (s_use_point_shadows == 0) {
        point_shadow = 0.0;
    }
    if (s_use_direction_shadow == 0) {
        direction_shadow = 0.0;
    }

    light_result += (1.0 - clamp(direction_shadow + point_shadow, 0.0, 1.0));
    //color = vec4(light_result, 1);
    //color = vec4((1.0 - clamp(direction_shadow + point_shadow, 0.0, 1.0)), 0, , 1);
    //color = vec4(light_result, 1);
    /*
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
    */
    
    /*
    if (use_uniform_color) {
          color = (vec4(uniform_color, 1) * vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
    } else {
        color = (texture(texture_diffuse1, TexCoord) + vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
    }
    */
}