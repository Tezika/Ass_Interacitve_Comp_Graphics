#version 430 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_local;
layout( location = 2 ) in vec2 i_texCoord;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;

out vec2 texCoord;
// out vec3 fragPos;
// out vec3 lightPos;


// Entry Point
//============

void main()
{
	gl_Position = mat_projection * mat_view * mat_model * vec4(i_vertexPosition_local,1);
	// vec4 fragPos4 = mat_view * mat_model * vec4(i_vertexPosition_local, 1);
	// fragPos = vec3(fragPos4) / fragPos4.w;
	// vec4 lightPos4 = mat_view * mat_light * vec4(worldPos_light, 1);
	// lightPos = vec3(lightPos4)/ lightPos4.w;
	texCoord = i_texCoord;
}
