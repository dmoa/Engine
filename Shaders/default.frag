#version 120

uniform sampler2D pixel_texture;
uniform vec4 color_filter;

varying vec2 texture_position;

void main() {
    gl_FragColor = texture2D( pixel_texture, texture_position.st) * color_filter;
}