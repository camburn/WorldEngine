#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; // In world space
in vec4 FragPosLightSpace;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadow_map;
uniform samplerCube shadow_cube_map;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightPointPos;
uniform vec3 viewPos;

uniform bool debug_draw_texcoords = false;
uniform bool debug_draw_normals = false;
uniform bool debug_disable_lighting = false;

uniform bool use_shadows = false;
uniform bool use_uniform_color = false;
uniform vec3 uniform_color = vec3( 1.0f, 1.0f, 1.0f);

uniform int pcf_samples = 1;
uniform float shadow_map_bias = 0.005f;

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
			shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	return shadow;
}

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPointPos;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(shadow_cube_map, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= 25.0f;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows

    float shadow = currentDepth -  shadow_map_bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void main(){

	if (!use_shadows && use_uniform_color) {
		color = vec4(uniform_color, 1);
		return;
	}

	float ambientStrength = 0.2f;
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

	float shadow = ShadowCalculation(FragPosLightSpace, lightDir);
	//shadow += ShadowCalculation(FragPos);
	

	vec3 light_final = (ambientStrength + (1.0 - shadow) * (diffuse + specular)) * lightColor;

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
		color = (texture(texture_diffuse1, TexCoord) * vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
	}
}