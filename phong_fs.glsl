#version 330

uniform sampler2D tex;
uniform float ambientStrength = 0.4;
uniform float specularStrength = 0.5;

out vec4 pixelColor;

in vec2 iTC;
in vec4 iPosition;
in vec4 iNormal;
in vec4 viewerPos;
in vec4 lp;

vec3 calculateAmbient(vec4 color, float ambientConstant) {
	return color.rgb * ambientConstant;
}


vec3 calculateDiffuse(vec3 normals, vec4 color, vec4 lightPos, vec3 position) {
    vec3 direction = normalize(lightPos.xyz - position);
    float diffuseConstant = max(normals, direction), 0.0);
    return vec3(color.xyz * diffuseConstant);
}


vec3 calculateSpecular(vec3 viewer, vec3 position, vec4 color, vec4 lightPos, vec3 normals, float specular) {
    vec3 direction = normalize(lightPos.xyz - position);
    vec3 viewPos = normalize(viewer - position);
    vec3 reflection = reflect(-direction, normalize(normals));
    float specularConstant = pow(max(dot(viewPos, reflection), 0.0), 32);
    return (specular * specularConstant * color.xyz);
}


vec4 calculateColor(vec3 normals, vec4 color, vec4 lightPos, vec3 position, float ambientConstant, vec4 viewer, float specularConst) {
    vec3 ambient = calculateAmbient(color, ambientConstant);
    vec3 diffuse = calculateDiffuse(normals, color, lightPos, position);
    vec3 specular = calculateSpecular(viewer.xyz, position, color, lightPos, normals, specularConst);
    return vec4(ambient + diffuse + specular, color.a);
}


void main(void) {
	vec4 color = texture(tex,iTC);
	pixelColor = calculateColor(iNormal.xyz, color, lp, iPosition.xyz, ambientStrength, viewerPos, specularStrength);
}
