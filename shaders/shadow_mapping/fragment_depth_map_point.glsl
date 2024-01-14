#version 450

in vec4 frag_pos;

out vec4 out_color;

uniform vec3 light_pos;
uniform float far_plane;

void main() {
	float light_distance = length(frag_pos.xyz - light_pos);
	light_distance = light_distance / far_plane;
	gl_FragDepth = light_distance;
}