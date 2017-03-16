#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; // In world space

out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){
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

	vec3 light_final = (ambientStrength  + diffuse + specular) * lightColor;
	color = (texture(ourTexture, TexCoord) * vec4(objectColor, 1.0)) * vec4(light_final, 1.0);
}