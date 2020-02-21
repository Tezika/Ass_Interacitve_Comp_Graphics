#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec3 texCoord;
out vec4 o_color;

// For cubemap
uniform samplerCube tex_cubemap;

// Entry Point
//============

void main()
{
	o_color = texture(tex_cubemap, texCoord);
}