#version 430

#define MAX_POINT_LIGHTS 10

#define SAMPLECOUNT 64
#define SAMPLECOUNT_FLOAT 64.0
#define SAMPLECOUNT_D2 32
#define SAMPLECOUNT_D2_FLOAT 32.0
#define INV_SAMPLECOUNT (1.0 / SAMPLECOUNT_FLOAT)

in vec2 TexCoord;
in vec3 Normal;
in vec3 TestNormal;
in vec3 FragPos; // In world space
in vec4 FragPosLightSpace;
in vec4 PositionB;

out vec4 color;

struct PointLight {
    vec4 position;

    vec4 ambience;
    vec4 diffuse;
    vec4 specular;

    vec4 attributes;
};

struct DirectionLight {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Material {
    vec3 base_color;
    float shininess;
    uint diffuse_set;
    uint specular_set;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

struct LightResult {
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
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

layout(std430, binding=4) buffer light_data {
    PointLight point_lights[MAX_POINT_LIGHTS];
};

layout(std430, binding=3) buffer shader_uniforms {
    
    uvec4 s_debug_flags;
    // x = s_debug_draw_normals
    // y = s_debug_draw_texcoords
    // z = s_debug_draw_lighting
    // w = s_debug_draw_specular

    uvec4 s_shadow_flags;
    // x = s_use_point_shadows
    // y = s_use_direction_shadow
    // z = s_pcf_samples
    // w = UNUSED

    vec4 s_map_config;
    // x = s_shadow_map_bias
    // y = s_cube_map_bias
    // z = s_direction_shadow_map_sampler_id
    // w = s_point_shadow_cube_map_sampler_id

    vec4 s_map_ranges;
    // x = s_far_plane
    // y = UNUSED
    // z = UNUSED
    // w = UNUSED

    mat4 s_perspective_matrix;
    mat4 s_view_matrix;
    mat4 s_light_direction_projection_matrix;
    mat4 s_light_direction_view_matrix;
    mat4 s_light_direction_vp_matrix; // view pers matrix

    vec4 s_view_pers_matrix;
    vec4 s_view_pos;
    vec4 s_light_direction_pos;
    vec4 s_light_direction_color;
    vec4 s_light_point_pos;
    vec4 s_light_point_color;

    uvec4 s_state_flags;
    // x = _render_flag
    // y = _debug_flag
    // z = point_light_count
    // w = UNUSED

    DirectionLight direction_light;
    //InstanceUniforms instance_uniforms[];
};

uniform sampler2D shadow_map;
uniform samplerCube shadow_cube_map;
uniform sampler3D shadow_jitter_map;
uniform vec3 objectColor;

uniform float jitter_scale = 32.0;

uniform bool use_shadows = false;

uniform bool use_uniform_color = false;
uniform vec3 uniform_color = vec3( 1.0, 1.0, 1.0);

uniform float ambience_strength = 0.2;

uniform float far_plane = 25.0;

uniform Material material;

const vec3 sampling_disk[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

const int pcf_samples = 20;

float DirectionShadowCalculation(vec4 frag_pos_light_space) {

    vec3 frag_to_light = frag_pos_light_space.xyz - s_light_direction_pos.xyz;

    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    if (proj_coords.z > 1.0) { return 0.0; }
    proj_coords = proj_coords * 0.5 + 0.5;
    // Shadow biasing to remove shadow acne
    //float map_bias = 0.00005;// = s_shadow_map_bias;
    vec3 light_direction = direction_light.direction.xyz;
    //float bias = clamp(s_map_config.x * tan(acos(dot(Normal, light_direction))), 0.0, 0.01);

    //float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    //closest_depth = closest_depth * -2 / (25.0f - 1.0f) - (25.0f + 1.0f)/(25.0f - 1.0f);
    
    // PCF (shadow softening)
    //float shadow = current_depth > closest_depth  ? 1.0 : 0.0;
    float bias = max(0.05 * (1.0 - dot(Normal, light_direction)), s_map_config.x);
    //float shadow = current_depth - bias > closest_depth  ? 1.0 : 0.0;
    
    float shadow = 0.0;

    //float view_distance = length(light.position.xyz - frag_pos);
    float disk_radius = 0.05;
    for (int i = 0; i < pcf_samples; ++i) {
        float closest_depth = texture(shadow_map, proj_coords.xy + sampling_disk[i].xy * disk_radius).r;
        closest_depth *= far_plane;
        if(current_depth - s_map_config.x > closest_depth){
            shadow += 1.0;
        }
    }
    shadow /= float(pcf_samples);
    /*
    float shadow = 0.0;
    int s_pcf_samples = int(s_shadow_flags.z);
    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
    for (int x = -s_pcf_samples; x <= s_pcf_samples; ++x) {
        for (int y = -s_pcf_samples; y <= s_pcf_samples; ++y) {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 0.5f : 0.0f;
        }
    }
    */
    
    return shadow * 5;
}

float PointShadowCalculation(PointLight light, vec3 frag_pos) {

    // get vector between fragment position and light position
    vec3 frag_to_light = frag_pos - s_light_point_pos.xyz;

    float current_depth = length(frag_to_light);
    // test for shadows
    float shadow = 0.0;

    float view_distance = length(light.position.xyz - frag_pos);
    float disk_radius = (1.0 + (view_distance / far_plane)) / 25.0;
    for (int i = 0; i < pcf_samples; ++i) {
        float closest_depth = texture(shadow_cube_map, frag_to_light + sampling_disk[i] * disk_radius).r;
        closest_depth *= far_plane;
        if(current_depth - s_map_config.y > closest_depth){
            shadow += 1.0;
        }
    }
    shadow /= float(pcf_samples);
    return shadow;
}

LightResult calc_dir_light(DirectionLight light, vec3 normal, vec3 view_dir) {
    vec3 light_direction = normalize(-light.direction.xyz);
    // Calculate diffuse impact by getting dot product between lightDir and the surface normal
    float diff = max(dot(normal, light_direction), 0.0);
    // Calc light direction vewctor and reflect vector
    vec3 reflect_direction = reflect(-light_direction, normal);
    // Calculate Specular component
    float spec = pow(max(dot(view_dir, reflect_direction), 0.0), material.shininess);

    vec3 ambient = light.ambient.xyz * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specular = vec3(0.0);
    if (material.specular_set == 1) {
        specular = light.specular.xyz * spec * vec3(texture(material.texture_specular1, TexCoord));
    }
    LightResult result;
    result.ambient = ambient;
    result.diffuse = diffuse;
    result.specular = specular;
    return result;
    //return (ambient * diffuse * specular);
    //return (ambient + diffuse);
}

LightResult calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
    vec3 light_dir = normalize(light.position.xyz - frag_pos);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular component
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    // Attenuation
    float light_distance = length(light.position.xyz - frag_pos);
    float attenuation = 1.0 / (
        light.attributes.x + light.attributes.y * light_distance +
        light.attributes.z * (light_distance * light_distance)
    );

    // Combine
    vec3 ambient = light.ambience.xyz * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specular = vec3(0.0);
    if (material.specular_set == 1) {
        specular = light.specular.xyz * spec * vec3(texture(material.texture_specular1, TexCoord));
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    //return (ambient + diffuse + specular);
    LightResult result;
    result.ambient = ambient;
    result.diffuse = diffuse;
    result.specular = specular;
    return result;
}

void main(){
    {
        if (!use_shadows && use_uniform_color) {
            color = vec4(uniform_color, 1);
            s_state_flags.y = 0;
            return;
        }
        if (s_debug_flags.z == 1) {
            color = (texture(material.texture_diffuse1, TexCoord) * vec4(objectColor, 1.0));
            s_state_flags.y = 1;
            return;
        }
        if (s_debug_flags.y == 1) {
            color = vec4(TexCoord, 0.0, 1.0);
            s_state_flags.y = 2;
            return;
        }
        if (s_debug_flags.x == 1) {
            color = vec4(Normal, 1.0);
            s_state_flags.y = 3;
            return;
        }
        if (s_debug_flags.w == 1) {
            color = texture(material.texture_specular1, TexCoord);
            s_state_flags.y = 4;
            return;
        }
    }

    float specularStrength = 0.5f;
    float shininess = 32.0f;

    vec3 view_dir = normalize(s_view_pos.xyz - FragPos);
    vec3 normal = normalize(Normal);
    vec3 light_direction = normalize(s_light_direction_pos.xyz - FragPos);

    /*
    vec3 light_result = vec3(1.0, 0.0, 0.0);
    for (int i = 0; i == 1; i++) {
        PointLight point_light = point_lights[i];
        light_result = calc_point_light(point_light, normal, FragPos, view_dir);
        s_state_flags.x = 77;
    }
    */

    LightResult p_result;
    float point_shadow = 0.0;
    if (s_shadow_flags.x == 1) {
        p_result = calc_point_light(point_lights[0], normal, FragPos, view_dir);
        point_shadow = PointShadowCalculation(point_lights[0], FragPos);
    }
    LightResult d_result;
    float direction_shadow = 0.0;
    if (s_shadow_flags.y == 1) {
        d_result = calc_dir_light(direction_light, normal, view_dir);
        direction_shadow = DirectionShadowCalculation(FragPosLightSpace);
        
    }

    vec3 tex_color = texture(material.texture_diffuse1, TexCoord).rgb;
    vec3 lighting = (p_result.ambient + (1.0 - point_shadow) * (
        p_result.diffuse + p_result.specular)) * tex_color;

    vec3 dir_lighting = (d_result.ambient + (1.0 - direction_shadow) * (
        d_result.diffuse + d_result.specular)) * tex_color;  
    
    color = vec4(dir_lighting + lighting, 1);
}