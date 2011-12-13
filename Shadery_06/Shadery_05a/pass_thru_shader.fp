#version 330

uniform vec3 ambientLight;

struct pointLight {
    vec3  position;
    vec3  color;
    float attenuation0;
    float attenuation1;
    float attenuation2;
    float angle;
};

uniform pointLight light1;
uniform pointLight light2;

struct materialProperties {
    vec3  ambientColor;
    vec3  diffuseColor;
    vec3  specularColor;
    float specularExponent;
};

uniform materialProperties material;

out vec4 vFragColor;

smooth in vec3  positionInEyeSpace3;
smooth in vec3  normalInEyeSpace;


void main() {
	vFragColor = vec4(material.ambientColor, 1.0);
	
	vec3  toLight=light1.position-positionInEyeSpace3;
	float r=length(toLight);
	float intensity=100.0/(light1.attenuation0+light1.attenuation1*r+light1.attenuation2*r*r);
	vec3 lightDirection=normalize(toLight);
	float diffuse=max(0.0,dot(normalInEyeSpace,lightDirection));
	
	vFragColor += vec4(intensity*diffuse*material.diffuseColor*light1.color, 0);

	vec3  toLight2=light2.position-positionInEyeSpace3;
	float r2=length(toLight2);
	float intensity2=100.0/(light2.attenuation0+light2.attenuation1*r2+light2.attenuation2*r2*r2);
	vec3 lightDirection2=normalize(toLight2);
	float diffuse2=max(0.0,dot(normalInEyeSpace,lightDirection2));
	
	vFragColor += vec4(intensity2*diffuse2*material.diffuseColor*light2.color, 0);
	
	if(diffuse>0.0) {
		vec3 halfvector=normalize(lightDirection-normalize(positionInEyeSpace3));
		float specular=max(0.0,dot(halfvector,normalInEyeSpace));
		float fSpecular=pow(specular,material.specularExponent);
		vFragColor += vec4(intensity*fSpecular*light1.color*material.specularColor, 0);	
	}

	if(diffuse2>0.0) {
		vec3 halfvector2=normalize(lightDirection2-normalize(positionInEyeSpace3));
		float specular2=max(0.0,dot(halfvector2,normalInEyeSpace));
		float fSpecular2=pow(specular2,material.specularExponent);
		vFragColor += vec4(intensity2*fSpecular2*light1.color*material.specularColor, 0);	
	}	
	
	
	// vFragColor = vec4(1.0, 0, 0, 1.0);
}