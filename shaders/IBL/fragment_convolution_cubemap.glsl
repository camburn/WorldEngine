#version 120

varying vec3 f_localpos;

uniform samplerCube environment_map;

const float PI = 3.14159265359;

void main() {
    vec3 normal = normalize(f_localpos);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    float sample_delta = 0.025;
    float number_samples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sample_delta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sample_delta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangent_sample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * normal; 

            irradiance += textureCube(environment_map, sample_vec).rgb * cos(theta) * sin(theta);
            number_samples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(number_samples));

    gl_FragColor = vec4(irradiance, 1.0);
}