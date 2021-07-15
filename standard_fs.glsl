#version 330

uniform sampler2D tex;
uniform float ambientStrength = 0.4;
uniform vec4 downLight = vec4(0,1,-5,0);
uniform vec4 upLight = vec4(0,1,4.5,0);
uniform vec4 centerLight = vec4(0,1,0,0);

out vec4 pixelColor;

in vec2 iTC;
in vec3 iPosition;
in vec4 iNormal;

vec3 calculateAmbient(vec4 color, float ambientConstant) {
	return color.rgb * ambientConstant;
}


vec3 calculateDiffuse(vec3 norm, vec3 down, vec3 up, vec3 center, vec3 color, vec3 pos) {

	vec3 upBorder = normalize(up - pos);
	float diffUp = max(dot(norm, upBorder), 0.0);

	vec3 downBorder = normalize(down - pos);
	float diffDown = max(dot(-norm, downBorder), 0.0);

	vec3 middle = normalize(center - pos);
	vec3 middle2 = normalize((center - vec3(0,0,-0.25)) - pos);

	// Calculating attenuation
	float dist = length(center - pos);
	float atten = 1.0 / (1.0 + 0.35 * dist + 0.44 * (dist * dist));

	// Calculating diffuse light for middle of the board
	float diffCenter = max(dot(norm, middle), 0.0);
	float diffCenter2 = max(dot(-norm, middle2), 0.0);

	return ((diffDown + diffUp + (diffCenter * atten) + (diffCenter2 * atten)) * color);
}


vec4 calculateColor(vec4 color, float ambientConstant, vec4 norm, vec4 down, vec4 up, vec4 center, vec3 position) {
	vec3 ambient = calculateAmbient(color, ambientConstant);
	vec3 diffuse = calculateDiffuse(norm.xyz, down.xyz, up.xyz, center.xyz, color.xyz, position);
	return vec4((ambient + diffuse) * color.xyz, color.a);
}


void main(void) {
	vec4 ambientLight = vec4(1,1,1,1);
	vec4 color=texture(tex,iTC);
	pixelColor=calculateColor(color, ambientStrength, iNormal, downLight, upLight, centerLight, iPosition);
}
