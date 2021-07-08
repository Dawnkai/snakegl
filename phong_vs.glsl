#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec4 lp = vec4(0,0,1,0);

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
layout (location=2) in vec2 texcoord;
layout (location=3) in vec4 color;

out vec4 iColor;
out vec4 l;
out vec4 n;
out vec4 v;
out vec2 iTC;

void main(void) {
    l = normalize(V * lp - V * M * position);
    v = normalize(vec4(0,0,0,1) - V * M * position);
    n = normalize(V * M * normal);

    iColor = vec4(1,1,1,1);

    iTC = texcoord;
    gl_Position = P * V * M * position;
}
