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
uniform sampler1D tex_distribution0;
uniform sampler1D tex_distribution1;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 emitColor;

uniform int receiveShadow;
uniform int numOfSample_pcfFiltering;
uniform int numOfSample_blockerSearch;
uniform float bias_dirLightShadowMap;
uniform float lightSize;
uniform float near_plane;

uniform vec3 viewPos;
uniform vec3 lightPos;

vec2 texelSize;

vec2 randomDirection(sampler1D i_distribution, float i_u)
{
   return texture(i_distribution, i_u).xy * 2 - vec2(1);
}

float pcf_dirLight(vec3 i_projCoords, float i_uvRadius )
{
	float currentDepth = i_projCoords.z;
	float shadow = 0;
	for (int i = 0; i < numOfSample_pcfFiltering; i++)
	{
		float z = texture(tex_shadowMap, i_projCoords.xy + randomDirection(tex_distribution1, i / float(numOfSample_pcfFiltering)) * i_uvRadius).r;
		shadow += (z < (i_projCoords.z - bias_dirLightShadowMap)) ? 1 : 0;
	}
	shadow /= numOfSample_pcfFiltering;
	return shadow;
}

//////////////////////////////////////////////////////////////////////////
// this search area estimation comes from the following article: 
// http://developer.download.nvidia.com/whitepapers/2008/PCSS_DirectionalLight_Integration.pdf
float searchWidth(float i_lightSize, float i_receiverDistance)
{
	return i_lightSize * (i_receiverDistance - near_plane) / viewPos.z;
}

float findBlockerDistance_dirLight(vec3 i_projCoords, float i_lightSize)
{
	int numOfBlockers = 0;
	float averageBlockerDis = 0;
	float width_search = searchWidth(i_lightSize, i_projCoords.z);
	for (int i = 0; i < numOfSample_blockerSearch; i++)
	{
		float z = texture(tex_shadowMap, i_projCoords.xy + randomDirection(tex_distribution0, i / float(numOfSample_blockerSearch)) * width_search).r;
		if (z < (i_projCoords.z - bias_dirLightShadowMap))
		{
			numOfBlockers++;
			averageBlockerDis += z;
		}
	}
	if( numOfBlockers > 0 )
	{
		return averageBlockerDis /= numOfBlockers;
	}
	else
	{
		return -1;
	}
}

float pcss_dirLight(vec3 i_projCoords, float i_lightSize)
{
	// blocker search
	float blockerDistance = findBlockerDistance_dirLight(i_projCoords, i_lightSize);
	if (blockerDistance == -1)
		return 1;		

	// penumbra estimation
	float penumbraWidth = (i_projCoords.z - blockerDistance) / blockerDistance;

	// percentage-close filtering
	float uvRadius = penumbraWidth * i_lightSize * near_plane / i_projCoords.z;
	return 1 - pcf_dirLight(i_projCoords, uvRadius);
}

float calculateShadow(vec4 i_fragPosInLightSpace, float i_lightSize)
{
	vec3 projCoords = i_fragPosInLightSpace.xyz / i_fragPosInLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	return pcss_dirLight(projCoords, i_lightSize);
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
	texelSize = 1.0/textureSize(tex_shadowMap, 0);
	float shadowness = receiveShadow == 0 ? 0.0 : calculateShadow(fragPosInLightSpace, lightSize);
	vec3 lighting = ambient + shadowness *(diffuse + specular);

	o_color = vec4(lighting, 1);
}