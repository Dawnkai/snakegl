#version 330

uniform sampler2D tex;
uniform float ambientStrength = 0.4;
uniform vec4 downLight = vec4(0,1,-5,0);
uniform vec4 upLight = vec4(0,1,4.5,0);

out vec4 pixelColor;

in vec2 iTC;
in vec4 iPosition;
in vec4 iNormal;

vec3 calculateAmbient(vec4 color, float ambientConstant) {
	return color.rgb * ambientConstant;
}


vec3 calculateDiffuse(vec3 normals, vec4 color, vec4 down, vec4 up, vec3 position) {
    vec3 directionDown = normalize(down.xyz - position);
    vec3 directionUp = normalize(up.xyz - position);
    float diffDown = max(dot(normalize(normals), directionDown), 0.0);
    float diffUp = max(dot(normalize(normals), directionUp), 0.0);
    return vec3(color.xyz * (diffUp + diffDown));
}


vec4 calculateColor(vec3 normals, vec4 color, vec4 down, vec4 up, vec3 position, float ambientConstant) {
    vec3 ambient = calculateAmbient(color, ambientConstant);
    vec3 diffuse = calculateDiffuse(normals, color, down, up, position);
    return vec4(ambient + diffuse, color.a);
}


void main(void) {
	vec4 color = texture(tex,iTC);
	pixelColor = calculateColor(iNormal.xyz, color, downLight, upLight, iPosition.xyz, ambientStrength);
}
