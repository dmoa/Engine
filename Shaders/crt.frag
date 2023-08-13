#version 130

uniform sampler2D pixel_texture;
uniform double time_elapsed;

in vec2 texture_position;

out vec4 out_color;

vec2 RadialDistortion(vec2 coord, double dist) {
    vec2 cc = coord - 0.5;
    dist = dot(cc, cc) * dist + cos(time_elapsed * .3) * .01;
    return (coord + cc * (1.0 + dist) * dist);
}

void main() {

    vec2 tcr = RadialDistortion(texture_position, .24)  + vec2(.001, 0);
    vec2 tcg = RadialDistortion(texture_position, .20);
    vec2 tcb = RadialDistortion(texture_position, .18) - vec2(.001, 0);

    vec4 res = vec4(texture(pixel_texture, tcr).r, texture(pixel_texture, tcg).g, texture(pixel_texture, tcb).b, 1)
    - cos(tcg.y * 128. * 3.142 * 2) * .03
    - sin(tcg.x * 128. * 3.142 * 2) * .03;

    out_color = res * texture(pixel_texture, tcg).a;
}