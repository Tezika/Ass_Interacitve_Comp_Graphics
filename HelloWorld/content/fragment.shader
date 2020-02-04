#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec4 vertexNormalPositionInView; 
in  vec4 vertexPositionInView;
out vec4 o_color;

// Entry Point
//============

void main()
{
	vec4 up = vec4(0, 1, 0, 0);
	vec4 right = vec4(1, 0, 0, 0);
	float g = dot( vertexNormalPositionInView, up );
	g = clamp(g, 0, 1);
	float r = dot( vertexNormalPositionInView, right);
	r = clamp(r, 0, 1);
	o_color = vec4(r, g, 1.0f, 1.0f);
}
