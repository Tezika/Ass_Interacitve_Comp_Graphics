#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_local;
layout( location = 1 ) in vec3 i_vertexNormalPosition_local;
layout (location = 2)  in vec2 i_texCoord;

uniform mat4 mat_modelToProjection;
uniform mat4 mat_normalModelToView;
uniform mat4 mat_modelToView;

out vec3 normalInterp;
out vec3 vertexPos;
out vec2 texCoord;

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
	vec4 normalInterp4 = mat_normalModelToView * vec4(i_vertexNormalPosition_local, 1);
	normalInterp = vec3(normalInterp4) / normalInterp4.w;
	vec4 vertexPos4 = mat_modelToView * vec4(i_vertexPosition_local, 1);
	vertexPos = vec3(vertexPos4) / vertexPos4.w;
	texCoord = i_texCoord;
}
