#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_pos;
layout( location = 1 ) in vec3 i_normal;

uniform mat4 mat_modelToView;
uniform mat4 mat_normalModelToView;
uniform mat4 mat_modelToProjection;

out vec3 normalInterp;
out vec3 vertexPos;

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
	gl_Position =  mat_modelToProjection * vec4(i_pos, 1);
	vec4 normalInterp4 = mat_normalModelToView * vec4(i_normal, 1);
	normalInterp = vec3(normalInterp4) / normalInterp4.w;
	vec4 vertexPos4 = mat_modelToView * vec4(i_pos, 1);
	vertexPos = vec3(vertexPos4) / vertexPos4.w;
}
