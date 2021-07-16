#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
layout (location=2) in vec2 texcoord;

out vec2 iTC;
out vec4 iPosition;
out vec4 iNormal;

void main(void) {
    gl_Position = P*V*M*position;

    // FIX for broken .obj files, invert texcoord's Y coordinate
    vec2 iTexCoord = vec2(texcoord.x, texcoord.y * -1.0f);

    iTC = iTexCoord;
    iPosition = vec4(M * position);
    iNormal = normal;
}
