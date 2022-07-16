#version 130

uniform sampler2D pixel_texture;

in vec2 texture_position;

out vec4 out_color;

void main() {
    out_color = texture( pixel_texture, texture_position.st);
}