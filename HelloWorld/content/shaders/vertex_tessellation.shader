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
uniform mat4 mat_light;
uniform vec3 worldPos_light;

out vec4 v_position;
out vec2 v_texCoord;
out vec3 v_normal;

// Entry Point
//============

void main()
{
	// pass by a default normal;
	v_normal = vec3(0, 0, 1);
	v_position = vec4(i_vertexPosition_local, 1);
	v_texCoord = i_texCoord; 
}
