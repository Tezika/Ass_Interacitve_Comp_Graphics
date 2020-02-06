#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec3 normalInterp; 
in  vec3 vertexPos;
in  vec3 lightPos;
out vec4 o_color;

const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const vec3 ambientColor = vec3(0.1, 0.0, 0.0);
// Entry Point
//============

void main()
{
	vec3 normal = normalize(normalInterp);
	vec3 lightDir = normalize(lightPos - vertexPos);
	float lambertian = max(dot(lightDir,normal), 0.0);
	float specular = 0;
	if(lambertian > 0)
	{
		// Blinn - Phong
		vec3 viewDir = normalize(-vertexPos);
		vec3 halfDir = normalize( lightPos + viewDir);
		float specAngle = max(dot(halfDir, normal), 0.0);
		specular = pow(specAngle, 16.0);
	}
	  o_color = vec4(ambientColor + lambertian * diffuseColor + specular * specColor, 1.0);
}
