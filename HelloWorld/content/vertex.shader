#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()

// These values come from one of the VertexFormats::s3dObject that the vertex buffer was filled with in C code
layout( location = 0 ) in vec3 i_vertexPosition_local;

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
	// float x = i_vertexPosition_local.x * 0.05f;
	// float y = i_vertexPosition_local.y * 0.05f;
	// float z = i_vertexPosition_local.z * 0.05f;
	gl_Position = vec4(i_vertexPosition_local, 1);
}
