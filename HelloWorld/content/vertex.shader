#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_local;
layout( location = 1 ) in vec3 i_vertexNormalPosition_local;

uniform mat4 mat_modelToProjection;
uniform mat4 mat_modelToView;

out vec4 vertexNormalPositionInView;
out vec4 vertexPositionInView;

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
	 gl_Position =  mat_modelToProjection * vec4(i_vertexPosition_local, 1);
	 vertexNormalPositionInView = mat_modelToView * vec4(i_vertexNormalPosition_local, 1);
	 vertexPositionInView = mat_modelToView * vec4(i_vertexPosition_local, 1);
}
