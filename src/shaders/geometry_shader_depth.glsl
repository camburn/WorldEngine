#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform bool cube_matrix = false;
uniform mat4 light_matrix;
uniform mat4 light_cube_matrix[6];

out vec4 frag_pos; // FragPos from GS (output per emitvertex)
out int gl_Layer;

void main()
{   
    if (!cube_matrix) {
        // Just emit the regular vertices
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            frag_pos = gl_in[i].gl_Position;
            gl_Position = light_matrix * frag_pos;
            EmitVertex();
        }  
        return;
    }
    int gl_Layer;
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            frag_pos = gl_in[i].gl_Position;
            gl_Position = light_cube_matrix[face] * frag_pos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 