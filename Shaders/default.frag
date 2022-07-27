#version 120

uniform sampler2D pixel_texture;

varying vec2 texture_position;

// out vec4 out_color;

void main() {
    gl_FragColor = texture2D( pixel_texture, texture_position.st);
}