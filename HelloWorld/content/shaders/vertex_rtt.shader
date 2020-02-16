#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_ndc;
layout( location = 1)  in vec2 i_texCoord;
out vec2 texCoord;

uniform mat4 mat_modelToView;
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
	gl_Position = mat_modelToView * vec4(i_vertexPosition_ndc, 1);
	texCoord = i_texCoord;
}
