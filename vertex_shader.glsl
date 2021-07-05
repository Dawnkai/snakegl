#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 position;
in vec3 color;
in vec2 texcoord;
in vec3 normal;

out vec3 iPosition;
out vec3 iColor;
out vec2 iTexcoord;
out vec3 iNormal;

void main() {
    iPosition = vec4(modelMatrix * vec4(position, 1.f)).xyz;
    iColor = color;
    iTexcoord = vec2(texcoord.x, texcoord.y * -1.f);
    iNormal = mat3(modelMatrix) * normal;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.f);
}
