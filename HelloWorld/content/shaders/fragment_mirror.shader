#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec2 texCoord;
out vec4 o_color;

// For renderTex
uniform sampler2D tex_mirror;
uniform sampler2D tex_environment;

// Entry Point
//============

void main()
{
	vec4 mixColor = texture(tex_mirror, texCoord) * texture(tex_environment, texCoord);
	if(length(mixColor.xyz) == 0)
	{
		o_color = vec4(texture(tex_environment, texCoord).xyz, 1);
	}
	else
	{
		o_color = vec4(mixColor.xyz, 1);
	}
}