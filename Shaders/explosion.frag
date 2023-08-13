#version 120

uniform sampler2D pixel_texture;
uniform double u_time;
uniform vec2 texture_offset;
uniform vec2 texture_size;

varying vec2 texture_position;

void main() {
    vec2 uv = (texture_position - texture_offset) / texture_size;

    // Compute center based on texture_offset
    vec2 center = vec2(0.5) - (texture_offset / texture_size);

    // Displacement direction (from center to current pixel)
    vec2 direction = normalize(uv - center);

    // Calculate displacement based on u_time
    vec2 displacement = u_time > 0.0 ? direction * u_time * 0.25 : vec2(0.0);

    // Offset the UV coordinates by the displacement
    vec2 newUV = uv + displacement;

    // Get the color from the new position
    vec4 color = texture2D(pixel_texture, newUV);

    // Decrease alpha over time to create a fade out effect
    color.a *= (1.0 - u_time);

    gl_FragColor = color;
}
