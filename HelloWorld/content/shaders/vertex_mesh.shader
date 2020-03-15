#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_local;
layout( location = 1 ) in vec3 i_vertexNormalPosition_local;
layout( location = 2 ) in vec2 i_texCoord;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;

out vec3 normalInterp;
out vec3 fragPos;
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
	gl_Position = mat_projection * mat_view * mat_model * vec4(i_vertexPosition_local, 1);
	normalInterp = transpose(inverse(mat3(mat_model))) * i_vertexNormalPosition_local;
	vec4 fragPos4 = mat_view * mat_model * vec4(i_vertexPosition_local, 1);
	fragPos = vec3(fragPos4) / fragPos4.w;
	texCoord = i_texCoord;
}
