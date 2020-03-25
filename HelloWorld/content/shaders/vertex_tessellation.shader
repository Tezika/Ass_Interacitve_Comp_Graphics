#version 430 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_local;
layout( location = 2 ) in vec2 i_texCoord;

out vec2 v_texCoord;
out vec4 v_position;

// Entry Point
//============

void main()
{
	v_position = vec4(i_vertexPosition_local, 1);
	v_texCoord = i_texCoord; 
}
