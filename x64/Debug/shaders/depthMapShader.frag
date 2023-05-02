// #version 410 core

// out vec4 fColor;
// in vec4 fragPosLightSpace;

// in vec3 fNormal;
// in vec4 fPosEye;
// in vec2 fTexCoords;
// in vec4 lightPosEye;


// uniform	vec3 lightDir;
// uniform	vec3 lightColor;
// uniform sampler2D diffuseTexture;
// uniform sampler2D specularTexture;
// uniform sampler2D ambientTexture;

// uniform sampler2D shadowMap;

// vec3 ambient;
// float ambientStrength = 0.2f;
// vec3 diffuse;
// vec3 specular;
// float specularStrength = 0.5f;
// float shininess = 32.0f;
// vec3 normalEye;

// void computeLightComponents()
// {		

	// float constant = 1.0f;
	// float linear = 0.0045f;
	// float quadratic = 0.0075f;

	// vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	// //transform normal
	// normalEye = normalize(fNormal);	
	
	// //compute light direction
	// //vec3 lightDirN = normalize(lightDir);
	// vec3 lightDirN = normalize(lightPosEye.xyz - fPosEye.xyz);
	
	// //compute view direction 
	// vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	// //compute half vector
	// vec3 halfVector = normalize(lightDirN + viewDirN);
	
	// //compute distance to light
	// float dist = length(lightPosEye.xyz - fPosEye.xyz);
	// //compute attenuation
	// float att = 1.0f / (constant + linear*dist + quadratic*(dist*dist));
		
	// //compute ambient light
	// ambient = att * ambientStrength * lightColor;
	
	// //ambient *= (texture(ambientTexture, fTexCoords)).xyz;
	// ambient *= (texture(diffuseTexture, fTexCoords)).xyz;
	
	
	// //compute diffuse light
	// diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	// diffuse *= (texture(diffuseTexture, fTexCoords)).xyz;
	
	// //compute specular light
	// //vec3 reflection = reflect(-lightDirN, normalEye);
	// //float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	// float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	// specular = att * specularStrength * specCoeff * lightColor;
	// specular *= (texture(specularTexture, fTexCoords)).xyz;
// }

// float computeShadow() {
	
	// //perform perspective divide
	// vec3 normalizedCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
	// normalizedCoords = normalizedCoords*0.5 + 0.5;
	// float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	// float currentDepth = normalizedCoords.z;
	
	// float bias = max(0.05f * (1.0f - dot(normalEye, lightDir)), 0.005f);
	
	// float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	// if(normalizedCoords.z > 1.0f)
		// return 0.0f;
	
	// return shadow;
// }

// void main()
// {
	// computeLightComponents();
	
	// vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	// ambient *= baseColor;
	// diffuse *= baseColor;
	// specular *= baseColor;
	
	// float shadow = computeShadow();
	
	// vec3 color = min((ambient + (1.0f + shadow) * diffuse) + (1.0f-shadow)*specular, 1.0f);
	
	// fColor = vec4(color, 1.0f);
// }

#version 410 core
out vec4 fColor;
void main()
{
	fColor = vec4(1.0f);
}
