#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; // In world space
in vec4 FragPosLightSpace;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadow_map;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool debug_draw_texcoords = false;
uniform bool debug_draw_normals = false;
uniform bool debug_disable_lighting = false;

uniform bool use_shadows = false;
uniform bool use_uniform_color = false;
uniform vec3 uniform_color = vec3( 1.0f, 1.0f, 1.0f);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir) {
	vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	if (proj_coords.z > 1.0) { return 0.0; }
	proj_coords = proj_coords * 0.5 + 0.5;
	float closest_depth = texture(shadow_map, proj_coords.xy).r;
	float current_depth = proj_coords.z;
	// Shadow biasing to remove shadow acne
	float bias = max(0.005 * (1.0 - dot(Normal, lightDir)), 0.0005);
	bias = clamp(0.005*tan(acos(dot(Normal, lightDir))), 0, 0.01);
	float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
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