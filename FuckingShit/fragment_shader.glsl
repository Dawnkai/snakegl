#version 330

struct Material {
    vec3 ambientLight;
    vec3 diffuseLight;
    vec3 specularLight;
    sampler2D diffuseTex;
    sampler2D specularTex;
};

in vec3 iPosition;
in vec3 iColor;
in vec2 iTexcoord;
in vec3 iNormal;

out vec4 rColor;

uniform sampler2D texture0;
uniform vec3 lightPos0;
uniform vec3 cameraPosition;
uniform Material material;


vec4 getDiffuseLight(Material material, vec3 iPosition, vec3 lightPos0, vec3 iNormal) {
    float diffuse = clamp(dot(normalize(lightPos0 - iPosition), iNormal), 0, 1);
    return vec4(material.diffuseLight * diffuse, 1.0f);
}


vec4 getSpecularLight(Material material, vec3 lightPos0, vec3 iPosition, vec3 iNormal, vec3 cameraPosition) {
    vec3 reflection = normalize(reflect(normalize(iPosition - lightPos0), normalize(iNormal)));
    float specularConstant = pow(max(dot(normalize(cameraPosition - iPosition), reflection), 0), 35);
    return vec4(material.specularLight * specularConstant, 1.0f);
}

vec4 getAmbientLight(Material material) {
    return vec4(material.ambientLight, 1.0f);
}


void main() {
    vec4 ambient = getAmbientLight(material);
    vec4 specular = getSpecularLight(material, lightPos0, iPosition, iNormal, cameraPosition);
    vec4 diffuse = getDiffuseLight(material, iPosition, lightPos0, iNormal);
    rColor = texture(texture0, iTexcoord) * (ambient + diffuse + specular);
}
