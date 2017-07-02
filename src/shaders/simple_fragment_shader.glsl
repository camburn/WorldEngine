#version 330 core
in vec3 frag_color;

uniform bool use_uniform_color;
uniform vec3 uniform_color;

out vec4 color;

void main()
{
    if (use_uniform_color) {
        color = vec4(uniform_color, 1.0f); // Set all 4 vector values to 1.0f
    }
    else {
        color = vec4(frag_color, 1.0f); // Set all 4 vector values to 1.0f
    }
}