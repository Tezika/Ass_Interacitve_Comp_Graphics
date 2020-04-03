#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in vec3 normalInterp; 
in vec3 fragPos;
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

uniform int receiveShadow;
uniform int width_pcfFiltering;
uniform int width_blockerSearch;
uniform float bias_dirLightShadowMap;

uniform vec3 viewPos;
uniform vec3 lightPos;

float pcf_dirLight(vec3 projCoords, float i_bias)
{
	float currentDepth = projCoords.z;
	float shadow = 0;
	float numOfSample = 0;
	vec2 texelSize = 1.0/textureSize(tex_shadowMap, 0);
	for(int x = -width_pcfFiltering; x <= width_pcfFiltering; x++)
	{
		for(int y = -width_pcfFiltering; y <= width_pcfFiltering; y++)
		{
			float pcfDepth = texture(tex_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
        	shadow += currentDepth - i_bias > pcfDepth ? 1.0 : 0.0; 
			numOfSample += 1.0;
		}
	}
	shadow /= numOfSample;
	return shadow;
}

float findBlockerDistance_dirLight(vec3 projCoords)
{
	float numOfSample = 0;
	vec2 texelSize = 1.0/textureSize(tex_shadowMap, 0);
	for(int x = -width_blockerSearch; x <= width_blockerSearch; x++)
	{
		for(int y = -width_blockerSearch; y <= width_blockerSearch; y++)
		{
			numOfSample += 1.0;
		}
	}
	return 0;
}

float penumbra_Estimate_dirLight()
{
	return 0;
}

float calculateShadow(vec4 i_fragPosInLightSpace, float i_bias)
{
	vec3 projCoords = i_fragPosInLightSpace.xyz / i_fragPosInLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	return pcf_dirLight( projCoords, i_bias );
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
	vec3 viewDir = normalize( viewPos - fragPos );
	vec3 halfDir = normalize( lightPos + viewDir);
	float specAngle = max(dot(halfDir, normal), 0.0);
	spec = pow(specAngle, 64.0);
	vec3 ambient = ambientColor;
	vec3 diffuse = diffuseColor * diff;
	vec3 specular = specularColor * spec;

	// Calulate shadow
	float shadow = receiveShadow == 0 ? 0.0 : calculateShadow(fragPosInLightSpace, bias_dirLightShadowMap);
	vec3 lighting = ambient + (1-shadow) *(diffuse + specular);
	o_color = vec4(lighting, 1);
}