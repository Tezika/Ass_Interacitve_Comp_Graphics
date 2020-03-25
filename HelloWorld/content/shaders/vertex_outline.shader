#version 330 core
// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()
layout( location = 0 ) in vec3 i_vertexPosition_local;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;

out vec4 v_position;
out vec3 v_normal;
out vec2 v_texCoord;

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
	gl_Position = mat_projection * mat_view * mat_model * vec4(i_vertexPosition_local,1);
	v_position = gl_Position;
	v_normal = vec3(1, 0, 0);
	v_texCoord = vec2(0,0);
}
