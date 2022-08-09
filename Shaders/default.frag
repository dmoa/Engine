#version 120

uniform sampler2D pixel_texture;
uniform float opacity;

varying vec2 texture_position;

void main() {
    gl_FragColor = texture2D( pixel_texture, texture_position.st) * vec4(1.0, 1.0, 1.0, opacity);
}