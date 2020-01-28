#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()
{
	// Output solid white
	o_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
