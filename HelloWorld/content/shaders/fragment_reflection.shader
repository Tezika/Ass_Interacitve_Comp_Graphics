#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in vec3 normalInterp;
in vec3 vertexPos;
in vec3 lightPos;
in vec2 texCoord;
out vec4 o_color;

// For cubemap
uniform samplerCube tex_cubemap;
uniform sampler2D tex_mirror;
uniform int mirroring;
uniform int enviroment;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 emitColor;

// Entry Point
//============

void main()
{
    vec3 normal = normalize(normalInterp);
	vec3 viewDir_reflect = normalize(vertexPos);
    vec3 r = reflect(viewDir_reflect, normal);
    vec3 reflectEnvColor = texture(tex_cubemap, r).rgb;
	vec3 mirrorColor;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	if(mirroring == 1)
	{
		mirrorColor = texture(tex_mirror, texCoord).xyz;
	}
 	// Blinn - Phong
	vec3 lightDir = normalize(lightPos - vertexPos);
	float diff = max(dot(lightDir,normal), 0.0);

	float spec = 0;
	vec3 viewDir = normalize(-vertexPos);
	vec3 halfDir = normalize( lightPos + viewDir);
	float specAngle = max(dot(halfDir, normal), 0.0);
	spec = pow(specAngle, 16.0);

    // Combined with the reflect color
	if(mirroring == 0)
	{
		ambient = ambientColor * reflectEnvColor;
		diffuse =  diff * diffuseColor * reflectEnvColor;
		specular = spec * specularColor * reflectEnvColor;
	}
	else
	{
		ambient = ambientColor * mirrorColor * reflectEnvColor;
		diffuse =  diff * diffuseColor * mirrorColor * reflectEnvColor;
		specular = spec * specularColor * mirrorColor * reflectEnvColor;
	}
	o_color = vec4( ambient + diffuse + specular, 1);
}