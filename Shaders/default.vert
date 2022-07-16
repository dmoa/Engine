#version 130

out vec2 texture_position;
out vec2 screen_position;

void main() {
    gl_Position = gl_Vertex;

    texture_position = gl_MultiTexCoord0.st;
    screen_position = gl_Vertex.xy;
}