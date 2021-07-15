#version 330

uniform sampler2D tex;
uniform float ambientStrength = 0.4;
uniform vec4 lightPos=vec4(0,1,0,0);

out vec4 pixelColor;

in vec2 iTC;
in vec4 iPosition;
in vec4 iNormal;

vec3 calculateAmbient(vec4 color, float ambientConstant) {
	return color.rgb * ambientConstant;
}


vec3 calculateDiffuse(vec3 normals, vec4 color, vec4 lightPos, vec3 position) {
    vec3 direction = normalize(lightPos.xyz - position);
    float diffuseConstant = max(dot(normalize(normals), direction), 0.0);
    return vec3(color.xyz * diffuseConstant);
}


vec4 calculateColor(vec3 normals, vec4 color, vec4 lightPos, vec3 position, float ambientConstant) {
    vec3 ambient = calculateAmbient(color, ambientConstant);
    vec3 diffuse = calculateDiffuse(normals, color, lightPos, position);
    return vec4(ambient + diffuse, color.a);
}


void main(void) {
	vec4 color = texture(tex,iTC);
	pixelColor = calculateColor(iNormal.xyz, color, lightPos, iPosition.xyz, ambientStrength);
}
