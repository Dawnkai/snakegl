#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
layout (location=2) in vec2 texcoord;
layout (location=3) in vec4 applePos;
layout (location=4) in vec4 snakePos;

out vec2 iTC;
out vec3 iPosition;
out vec4 iNormal;

void main(void) {
    gl_Position = P*V*M*position;

    iPosition = vec3(M * position);
    iNormal = normalize(normal);    
    iTC = texcoord;
}
