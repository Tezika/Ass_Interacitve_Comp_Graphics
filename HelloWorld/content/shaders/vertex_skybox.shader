#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_pos;
out vec3 texCoord;

uniform mat4 mat_view;
uniform mat4 mat_proj;
// Output
//=======
// layout( location = 1) out vec4 o_vertexColor;
// The vertex shader must always output a position value,
// but unlike HLSL where the value is explicit
// GLSL has an automatically-required variable named "gl_Position"

// Entry Point
//============

void main()
{
	gl_Position =  mat_proj * mat_view * vec4(i_pos, 1);
	// gl_Position = pos.xyww; 
	texCoord = i_pos;
}
