#version 330

uniform sampler2D tex;
uniform float ambientStrength = 0.4;

out vec4 pixelColor;

in vec2 iTC;
in vec3 iPosition;
in vec4 downLight;
in vec4 upLight;
in vec4 iNormal;

vec3 calculateAmbient(vec4 color, float ambientConstant) {
	return color.rgb * ambientConstant;
}


vec3 calculateDiffuse(vec3 norm, vec3 down, vec3 up, vec3 color, vec3 pos) {

	vec3 upBorder = normalize(up - pos);
	float diffUp = max(dot(norm, upBorder), 0.0);

	vec3 downBorder = normalize(down - pos);
	float diffDown = max(dot(-norm, downBorder), 0.0);

	return (diffDown + diffUp) * color;
}


vec4 calculateColor(vec4 color, float ambientConstant, vec4 norm, vec4 down, vec4 up, vec3 position) {
	vec3 ambient = calculateAmbient(color, ambientConstant);
	vec3 diffuse = calculateDiffuse(norm.xyz, down.xyz, up.xyz, color.xyz, position);
	return vec4((ambient + diffuse) * color.xyz, color.a);
}


void main(void) {
	vec4 ambientLight = vec4(1,1,1,1);
	vec4 color=texture(tex,iTC);
	pixelColor=calculateColor(color, ambientStrength, iNormal, downLight, upLight, iPosition);
}
