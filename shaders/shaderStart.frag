#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec3 fPosition;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform vec3 light2Dir;
uniform	vec3 lightColor;
uniform vec3 light2Color;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform int withFog;
uniform int night;
uniform int felinar;

vec3 ambient1 = vec3(0.0f);
vec3 ambient2 = vec3(0.0f);
float ambientStrength = 0.2f;
vec3 diffuse1 = vec3(0.0f);
vec3 diffuse2 = vec3(0.0f);
vec3 specular1 = vec3(0.0f);
vec3 specular2 = vec3(0.0f);
float shadow;
float specularStrength = 0.5f;
float shininess = 32.0f; 
vec4 fogColor;

float constant = 0.001f;
float linear = 0.00045f;
float quadratic = 0.0075f;


float computeShadow() 
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	
	float bias = max(0.05f * (1.0f - dot(normalize(fNormal), lightDir)), 0.005f);
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	return shadow;
}

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient1 = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse1 = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular1 = specularStrength * specCoeff * lightColor;
}

void computePointLightComponents(vec3 lDir) {

	vec3 cameraPosEye = vec3(0.0f);

	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lDir);
	
	//compute view direction 
	//vec3 viewDirN = -normalize(cameraPosEye - fPosEye.xyz);
	
	//compute distance to light
	float dist = length(lDir - fPosition.xyz) * 0.2;
	
	if(dist < 0.5) {
		dist = -10;
		}
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
	
	//compute ambient light
	ambient2 = att * ambientStrength * light2Color;
	
	//compute diffuse light
	diffuse2 = att * max(dot(normalEye, lightDirN), 0.0f) * light2Color;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(lightDirN, reflection), 0.0f), shininess);
	specular2 = att * specularStrength * specCoeff * light2Color;
}


float computeFog()
{
	float fogDensity = 0.012f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance*fogDensity, 2));
	
	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	shadow = computeShadow();
	computeLightComponents();
	if(felinar == 1)
		computePointLightComponents(light2Dir);
	float fogFactor = computeFog();
	if(night == 1)
		fogColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	else
		fogColor = vec4(0.4f, 0.4f, 0.4f, 1.0f);
	
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient1 *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse1 *= texture(diffuseTexture, fTexCoords).rgb;
	specular1 *= texture(specularTexture, fTexCoords).rgb;
	
	ambient2 *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse2 *= texture(diffuseTexture, fTexCoords).rgb;
    specular2 *= texture(specularTexture, fTexCoords).rgb;

	vec3 color = min(((ambient1+ambient2) + (1.0f - shadow)*(diffuse1+diffuse2)) + (1.0f - shadow)*(specular1+specular2), 1.0f);
	
	if(withFog == 1) {
		fColor = fogColor * (1 - fogFactor) + vec4(color, 1.0f) * fogFactor;
	} else {
		fColor = vec4(color, 1.0f);
	}
	
	
}
