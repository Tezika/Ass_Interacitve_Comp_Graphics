#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec4 vertexNormalPositionInView; 
out vec4 o_color;

// Entry Point
//============

void main()
{
	vec4 up = vec4(0, 1, 0, 0);
	vec4 right = vec4(1, 0, 0, 0);
    if(dot( vertexNormalPositionInView, right ) > 0)
	{
		o_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if(dot( vertexNormalPositionInView, up ) > 0)
	{
		o_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		o_color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
}
