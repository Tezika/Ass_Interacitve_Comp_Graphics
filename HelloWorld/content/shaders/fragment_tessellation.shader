#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen

in vec3 fragPos;
in vec2 texCoord;
out vec4 o_color;

// For diffuse
uniform sampler2D tex_diff;
// For specular
uniform sampler2D tex_spec;
// For normal map
uniform sampler2D tex_normalMap;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;

uniform vec3 lightPos;

// Entry Point
//============

void main()
{
	// calculate normal based on the normal map
	// obtain normal from normal map in range [0,1]
    vec3 normal = texture(tex_normalMap, texCoord).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  

	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir,normal), 0.0);
	// Blinn - Phong shading
	float spec = 0;
	vec3 viewDir = normalize(-fragPos);
	vec3 halfDir = normalize(lightPos + viewDir);
	float specAngle = max(dot(halfDir, normal), 0.0);
	spec = pow(specAngle, 16.0);
	vec3 ambient = ambientColor;
	vec3 diffuse =  diffuseColor * diff;
	vec3 specular = specularColor * spec;
	o_color = vec4( ambient + diffuse + specular, 1);
}