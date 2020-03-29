#version 430 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen

in vec3 fragPos;
in vec2 texCoord;
in vec4 fragPosInLightSpace;
out vec4 o_color;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;

uniform sampler2D tex_normal;
uniform sampler2D tex_shadowMap;
uniform int shadowing;

uniform vec3 viewPos;
uniform vec3 lightPos;

// Entry Point
//============
float calculateShadow(vec4 i_fragPosInLightSpace, float i_bias)
{
	vec3 projCoord = i_fragPosInLightSpace.xyz / i_fragPosInLightSpace.w;
	projCoord = projCoord * 0.5 + 0.5;
	float closestDepth  = texture(tex_shadowMap, projCoord.xy).z;
	float currentDepth = projCoord.z;
	float shadow = currentDepth - i_bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main()
{
	// calculate normal based on the normal map
	// obtain normal from normal map in range [0,1]
    vec3 normal = texture(tex_normal, texCoord).rgb;
    // transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir,normal), 0.0);
	// Blinn - Phong shading
	float spec = 0;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfDir = normalize(lightPos + viewDir);
	float specAngle = max(dot(halfDir, normal), 0.0);
	spec = pow(specAngle, 16.0);
	vec3 ambient = ambientColor;
	vec3 diffuse =  diffuseColor * diff;
	vec3 specular = specularColor * spec;
	float shadow = shadowing == 0 ? 0.0 : calculateShadow(fragPosInLightSpace, 0.001);
	vec3 lighting = ambient + (1-shadow) *(diffuse + specular);
	o_color = vec4(lighting, 1);
}