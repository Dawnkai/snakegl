#version 330

uniform sampler2D tex;

out vec4 pixelColor;

in float iNL;
in vec2 iTC;

void main(void) {
	vec4 color=texture(tex,iTC);
	pixelColor=vec4(color.rgb*iNL,color.a);
}
