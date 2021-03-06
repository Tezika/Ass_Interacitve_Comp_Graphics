#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen

in vec3 fragPos;
in vec3 normalInterp;
in vec2 texCoord;

out vec4 o_color;

// For diffuse
uniform sampler2D tex_diff;
// For specular
uniform sampler2D tex_spec;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 emitColor;
uniform int  texturing;

uniform vec3 lightPos;

// Entry Point
//============

void main()
{
	// Blinn - Phong
	vec3 normal = normalize(normalInterp);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir,normal), 0.0);


	float spec = 0;
	vec3 viewDir = normalize(-fragPos);
	vec3 halfDir = normalize(lightPos + viewDir);
	float specAngle = max(dot(halfDir, normal), 0.0);
	spec = pow(specAngle, 16.0);
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	if(texturing == 1)
	{
		ambient = ambientColor * vec3(texture(tex_diff, texCoord));
		diffuse =  diffuseColor * diff * vec3(texture(tex_diff, texCoord));
		specular = specularColor * spec * vec3( texture(tex_spec, texCoord));
	}
	else
	{
		ambient = ambientColor;
		diffuse =  diffuseColor * diff;
		specular = specularColor * spec;
	}
	o_color = vec4( ambient + diffuse + specular, 1);
}