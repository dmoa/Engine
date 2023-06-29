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
const float linear = 9999999999.0;
const float quadratic = 0.0;

const float distance_fine_to_draw = 0.7;

varying vec2 texture_position;
varying vec2 screen_position;

void main() {
    // vec3 color = vec3(0.0);
    // float totalAttenuation = 0.0;

    // for (int i = 0; i < num_lights; i++) {

    float distance = length(lights[0].position - texture_position);

    if (distance < distance_fine_to_draw) {
        gl_FragColor = texture2D(pixel_texture, texture_position.st);;
    }
    else {
        gl_FragColor = vec4(0.0);
    }
        // distance = distance / (lights[i].power);
        // float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        // float attenuation = 1.0 / (distance);
        // totalAttenuation += attenuation;
        // color += lights[i].color * attenuation;
    // }

    // color = clamp(color, 0.0, 1.0);
    // totalAttenuation = clamp(totalAttenuation, 0.0, 1.0);
    // gl_FragColor = pixel * vec4(color, totalAttenuation);

}
