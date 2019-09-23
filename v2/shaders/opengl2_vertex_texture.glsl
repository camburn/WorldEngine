#version 130

attribute vec4 position;
attribute vec4 normal;
attribute vec2 texcoord;
//attribute vec3 tangent;

uniform mat4 u_view_projection;
uniform mat4 u_model;
uniform mat4 u_light_space_matrix;

varying vec3 f_normal;
varying vec3 f_worldpos;
varying vec2 f_texcoord;
varying vec4 f_frag_pos_light_space;
//varying vec3 f_tangent;
//varying vec3 f_binormal;

void main() {
    f_normal = (u_model * normal).xyz;

    // Calculte Tangent Bitangent Normal Matrix
    //vec3 vertex_binormal = cross(normal.xyz, tangent);
    // Note: transpose(inverse(mat3(u_model))) is correct
    //mat3 normal_matrix = transpose(mat3(u_model));
    //f_tangent = normalize(normal_matrix * tangent);
    //f_tangent = normalize(f_tangent - dot(f_tangent, f_normal) * f_normal);
    //f_binormal = normalize(normal_matrix * vertex_binormal);
    //f_binormal = cross(f_normal, f_tangent);

    f_texcoord = texcoord;
    
    f_worldpos = (u_model * vec4(position.xyz, 1)).xyz;
    f_frag_pos_light_space = u_light_space_matrix * vec4(f_worldpos, 1);
    gl_Position = u_view_projection * u_model * position;
}