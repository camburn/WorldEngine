#version 330 core

in vec4 frag_pos;
// Is this an Nvidia - Intel difference?
//out float gl_FragDepth;

uniform vec3 light_pos;
uniform float far_plane;
uniform bool cube_matrix = false;

void main() {
    if (cube_matrix) {
        // get distance between fragment and light source
        float light_distance = distance(light_pos, frag_pos.xyz);
        
        // map to [0;1] range by dividing by far_plane
        light_distance = light_distance / far_plane;
        gl_FragDepth = light_distance;
    } else {
        float f_ndc_depth = frag_pos.z / frag_pos.w;
        gl_FragDepth = (1.0 - 0.0) * 0.5 * f_ndc_depth + (1.0 + 0.0) * 0.5;
    }
}