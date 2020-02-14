#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec2 texCoord;
out vec3 o_color;

// For renderTex
uniform sampler2D tex_render;
const float time = 0.1;

// Entry Point
//============

void main()
{
	o_color = texture( tex_render, texCoord + 0.005*vec2( sin(time+1024.0*texCoord.x),cos(time+768.0*texCoord.y)) ).xyz;
}