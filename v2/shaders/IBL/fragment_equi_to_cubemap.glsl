#version 120

varying vec3 f_localpos;
uniform sampler2D equirectangular_map;

const vec2 inv_atan = vec2(0.1591, 0.3183);

vec2 sample_spherical_map(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inv_atan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = sample_spherical_map(normalize(f_localpos));
    vec3 color = texture2D(equirectangular_map, uv).rgb;

    gl_FragColor = vec4(color, 1.0);
}