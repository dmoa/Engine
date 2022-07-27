#version 120

#define MAX_NUM_LIGHTS 64

struct Light {
    vec2 position; // normalised
    vec3 color;    // 0->1
    float power;
};

uniform sampler2D pixel_texture;
uniform float time_elapsed;

uniform int num_lights;
uniform Light lights[MAX_NUM_LIGHTS];

const float constant = 1.0;
const float linear = 0.09;
const float quadratic = 0.032;



varying vec2 texture_position;
varying vec2 screen_position;

// out vec4 return_color;



void main() {

    vec4 pixel = texture2D(pixel_texture, texture_position.st);
    vec3 color = vec3(0.3);

    for (int i = 0; i < num_lights; i++) {
        float distance = length(lights[i].position - texture_position) / lights[i].power;
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        color += lights[i].color * attenuation;
    }

    color = clamp(color, 0.0, 1.0);

    gl_FragColor = pixel * vec4(color, 1.0);

}