#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in vec3 normalInterp; 
in vec3 fragPos;
in vec3 lightPos;
in vec2 texCoord;
in vec4 fragPosInLightSpace;
out vec4 o_color;

// // For diffuse
// uniform sampler2D tex_diff;
// // For specular
// uniform sampler2D tex_spec;
uniform sampler2D tex_shadowMap;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 emitColor;
uniform int  shadowing;

float calculateShadow(vec4 i_fragPosInLightSpace)
{
	vec3 projCoord = i_fragPosInLightSpace.xyz / i_fragPosInLightSpace.w;
	projCoord = projCoord * 0.5 + 0.5;
	float closestDepth  = texture(tex_shadowMap, projCoord.xy).r;
	float currentDepth = projCoord.z;
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	return shadow;
}

// Entry Point
//============

void main()
{
	vec3 normal = normalize(normalInterp);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir,normal), 0.0);

	// Blinn - Phong
	float spec = 0;
	vec3 viewDir = normalize(-fragPos);
	vec3 halfDir = normalize( lightPos + viewDir);
	float specAngle = max(dot(halfDir, normal), 0.0);
	spec = pow(specAngle, 16.0);
	vec3 ambient = ambientColor;
	vec3 diffuse = diffuseColor * diff;
	vec3 specular = specularColor * spec;
	float shadow = shadowing == 0? 0 : calculateShadow(fragPosInLightSpace);
	o_color = vec4((ambient + (1 - shadow) * (diffuse + specular)), 1);
}