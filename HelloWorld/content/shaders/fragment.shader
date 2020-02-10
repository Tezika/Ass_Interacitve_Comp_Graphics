#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec3 normalInterp; 
in  vec3 vertexPos;
in  vec3 lightPos;
in  vec2 texCoord;
out vec4 o_color;

// For diffuse
uniform sampler2D texture1;
// For specular
uniform sampler2D texture2;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 emitColor;

// Entry Point
//============

void main()
{
	vec3 normal = normalize(normalInterp);
	vec3 lightDir = normalize(lightPos - vertexPos);
	float diff = max(dot(lightDir,normal), 0.0);

	// Blinn - Phong
	float spec = 0;
	vec3 viewDir = normalize(-vertexPos);
	vec3 halfDir = normalize( lightPos + viewDir);
	float specAngle = max(dot(halfDir, normal), 0.0);
	spec = pow(specAngle, 16.0);

	vec3 ambient = ambientColor * vec3(texture(texture1, texCoord));
	vec3 diffuse =  diffuseColor * diff * vec3(texture(texture1, texCoord));
	vec3 specular = specularColor * spec * vec3( texture(texture2, texCoord));

	o_color = vec4( ambient + diffuse + specular, 1);
}
