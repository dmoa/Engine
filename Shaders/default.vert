#version 120

varying vec2 texture_position;
varying vec2 screen_position;

void main() {
    gl_Position = gl_Vertex;

    texture_position = vec2(gl_MultiTexCoord0.s, gl_MultiTexCoord0.t);
    screen_position = gl_Vertex.xy;
}