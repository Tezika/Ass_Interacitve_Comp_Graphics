#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec2 texCoord;
out vec4 o_color;

// For renderTex
uniform sampler2D tex_mirror;

// Entry Point
//============

void main()
{
	o_color = vec4(texture(tex_mirror, texCoord).xyz, 1);
}