#version 330 core

in vec4 frag_pos;
out float gl_FragDepth;

uniform vec3 light_pos;
uniform float far_plane;
uniform bool cube_matrix = false;

void main(){
    /*
    if (cube_matrix) {
        // get distance between fragment and light source
        float light_distance = length(frag_pos.xyz - light_pos);
        
        // map to [0;1] range by dividing by far_plane
        light_distance = light_distance / far_plane;
        gl_FragDepth = light_distance;
    //gl_FragDepth = frag_pos.z;
    } else {
        return;
    }
    */
}