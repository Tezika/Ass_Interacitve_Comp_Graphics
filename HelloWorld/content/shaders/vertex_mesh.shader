#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_local;
layout( location = 1 ) in vec3 i_vertexNormalPosition_local;
layout( location = 2 )  in vec2 i_texCoord;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;
uniform mat4 mat_normalToView;
uniform mat4 mat_lightTransformation;
uniform mat4 mat_lightSpaceTransformation;

out vec3 normalInterp;
out vec3 vertexPos;
out vec3 lightPos;
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
	gl_Position =  mat_lightSpaceTransformation * mat_model * vec4(i_vertexPosition_local, 1);
	// vec4 normalInterp4 = mat_normalToView * vec4(i_vertexNormalPosition_local, 1);
	// normalInterp = vec3(normalInterp4) / normalInterp4.w;
	// vec4 vertexPos4 = mat_view * mat_model * vec4(i_vertexPosition_local, 1);
	// vertexPos = vec3(vertexPos4) / vertexPos4.w;
	// vec4 lightPos4 = mat_lightTransformation * vec4(0.0, 0.0, 0.0, 1.0);
	// lightPos = vec3(lightPos4)/lightPos4.w;
	// texCoord = i_texCoord;
}
