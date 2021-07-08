#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec4 lightDir=vec4(0,0,1,0);

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
layout (location=2) in vec2 texcoord;

out vec2 iTC;
out float iNL;

void main(void) {
    gl_Position = P*V*M*position;

    mat4 G=mat4(inverse(transpose(mat3(M))));
    vec4 n=normalize(V*G*normal);

    iNL=clamp(dot(n,lightDir),0,1);

    // FIX for broken .obj files, invert texcoord's Y coordinate
    vec2 iTexCoord = vec2(texcoord.x, texcoord.y * -1.0f);
    iTC=iTexCoord;
}
